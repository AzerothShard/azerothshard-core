#include "ScriptMgr.h"
#include "Unit.h"
#include "SharedDefines.h"
#include "Player.h"
#include "Common.h"
#include "raid.h"
#include "Creature.h"
#include "AzthPlayer.h"
#include "AzthLevelStat.h"
#include "AzthUtils.h"
#include "AzthAchievement.h"
#include "Pet.h"
#include "ScriptedGossip.h"
#include "LFG.h"
#include "LFGMgr.h"
#include "VASAutoBalance.h"
#include "MapManager.h"

typedef std::map<uint32, TwRaid> RaidList;
RaidList raidList;
std::map<uint32, AzthLevelStat> timeWalkingLevelsStatsList;
std::map<uint32, AzthAchievement> azthAchievementList;

enum npc_timewalking_enum
{
    TIMEWALKING_GOSSIP_NPC_TEXT_MAIN = 50100,
    TIMEWALKING_GOSSIP_NPC_TEXT_BONUS = 50101,
    TIMEWALKING_GOSSIP_NPC_TEXT_EXP = 50102,
    TIMEWALKING_GOSSIP_NPC_TEXT_PHASE = 50103,
    TIMEWALKING_GOSSIP_NPC_TEXT_RAID = 50104,
    TIMEWALKING_GOSSIP_NPC_TEXT_ALREADYAPPLIED = 50105,
	TIMEWALKING_GOSSIP_NPC_TEXT_ACTIVELFG = 50106,
	TIMEWALKING_GOSSIP_NPC_TEXT_INGROUP = 50107
};

class MapMythicInfo : public DataMap::Base
{
public:
    MapMythicInfo() {}
    MapMythicInfo(uint32 level) : mythicLevel(level){}
    uint32 mythicLevel = 0;
};


class TWVasScript: public VasModuleScript {
public:
    TWVasScript(): VasModuleScript("TWVasScript") {
    }
    
    bool OnBeforeModifyAttributes(Creature* creature, uint32 & /*instancePlayerCount*/) override {
        // it doesn't mean that we're allowing it when not in dungeon
        // but we're letting module to decide via its config.
        if (!creature->GetMap()->IsDungeon())
            return true;

        // Flexible content is allowed only for pre-wotlk instances and some specific wotlk
        uint32 expansion = creature->GetMap()->GetEntry()->Expansion();        
        uint32 mapId=creature->GetMap()->GetId();
        uint32 posLvl = sAzthUtils->getPositionLevel(false, creature->GetMap(), creature->GetZoneId(), creature->GetAreaId());
        if ((posLvl > 70 || expansion >= 2)
            && mapId != 603 // Ulduar
            && mapId != 533 // Naxxramas
            && mapId != 616 // The Eye of Eternity
            && mapId != 615 // Obsidian Sanctum
            && mapId != 249 // Onyxia's Lair
        )
            return false;

        Map::PlayerList const &playerList = creature->GetMap()->GetPlayers();
        if (!playerList.isEmpty())
        {
            for (Map::PlayerList::const_iterator playerIteration = playerList.begin(); playerIteration != playerList.end(); ++playerIteration)
            {
                if (Player* playerHandle = playerIteration->GetSource())
                {
                    if (playerHandle->IsGameMaster())
                        continue;
                    
                    //uint32 maxNumberOfPlayers = ((InstanceMap*)sMapMgr->FindMap(creature->GetMapId(), creature->GetInstanceId()))->GetMaxPlayers();
                    
                    //if (

                    //instancePlayerCount = playerHandle->azthPlayer->getGroupSize();

                    uint32 specialLevel =  playerHandle->azthPlayer->getPStatsLevel(false);
                    
                    if (sAzthUtils->isMythicLevel(specialLevel)) {
                        MapMythicInfo *myth=creature->GetMap()->CustomData.GetDefault<MapMythicInfo>("AZTH_Mythic_Info");
                        myth->mythicLevel = specialLevel;
                        return true;
                    }
                }
            }
        }

        return false;
    }
    
    bool OnBeforeUpdateStats(Creature* creature, uint32 &scaledHealth, uint32 &scaledMana, float &damageMultiplier, uint32 &newBaseArmor) override { 
        MapMythicInfo *myth=creature->GetMap()->CustomData.GetDefault<MapMythicInfo>("AZTH_Mythic_Info");
        
        
        if (myth->mythicLevel) {
            float rate = myth->mythicLevel - TIMEWALKING_LVL_VAS_START + 1;

            scaledHealth += rate > 1 ? scaledHealth*float(rate/10) : 1;
            scaledMana += rate > 1 ? scaledMana*float(rate/10) : 1;
            newBaseArmor += rate > 1 ? newBaseArmor*float(rate/10) : 1;
            damageMultiplier *= rate;

            return true;
        }
        
        return false; // something wrong....
    }
};

class loadTimeWalkingRaid : public WorldScript
{
public:
    loadTimeWalkingRaid() : WorldScript("loadTimeWalkingRaid") {}
    void OnStartup()
    {

        //Loading timewalking instance
        QueryResult timewalking_table = ExtraDatabase.PQuery("SELECT id,name,exp,phase,level,bonus,criteria FROM timewalking ORDER BY exp, phase, level, name;");
        if (!timewalking_table)
        {
            sLog->outString(">> Loaded 0 raids for TimeWalking. DB table `timewalking` is empty.\n");
            sLog->outString();
            return;
        }


        do
        {
            Field* timeWalking_Field = timewalking_table->Fetch();
            raidList[timeWalking_Field[0].GetUInt32()] = TwRaid(timeWalking_Field[0].GetUInt32(), timeWalking_Field[1].GetString(), timeWalking_Field[2].GetUInt32(), timeWalking_Field[3].GetUInt32(), timeWalking_Field[4].GetUInt32(), timeWalking_Field[5].GetUInt32(), timeWalking_Field[6].GetUInt32());
        } while (timewalking_table->NextRow());
        
        sAzthRaid->SetRaidList(raidList);
        
        //-------------------------------------------------------------------
        //Loading levels
        QueryResult timewalkingLevel_table = ExtraDatabase.PQuery("SELECT level,race,class,"
        "strength_pct,agility_pct,stamina_pct,intellect_pct,spirit_pct,"
        "dodge_pct, parry_pct, block_pct, crit_pct, armor_pen_pct, health_pct, resistance_pct, power_cost_pct, stat_pct,"
        "damage_pct,heal_pct FROM timewalking_levels ORDER BY level;");
        if (!timewalkingLevel_table)
        {
            sLog->outString(">> Loaded 0 levels for TimeWalking. DB table `timewalking_levels` is empty.\n");
            sLog->outString();
            return;
        }



        do
        {
            Field* timeWalkingLevel_Field = timewalkingLevel_table->Fetch();

            std::map<aura_timewalking_enum, uint32> _pctMap;
            _pctMap[TIMEWALKING_AURA_MOD_STR_PCT] = timeWalkingLevel_Field[3].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_AGI_PCT] = timeWalkingLevel_Field[4].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_STA_PCT] = timeWalkingLevel_Field[5].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_INT_PCT] = timeWalkingLevel_Field[6].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_SPI_PCT] = timeWalkingLevel_Field[7].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_DODGE_PCT] = timeWalkingLevel_Field[8].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_PARRY_PCT] = timeWalkingLevel_Field[9].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_BLOCK_PCT] = timeWalkingLevel_Field[10].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_CRIT_PCT] = timeWalkingLevel_Field[11].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_ARMOR_PENET_PCT] = timeWalkingLevel_Field[12].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_INCREASE_HEALTH_PCT] = timeWalkingLevel_Field[13].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_RESISTANCE_PCT] = timeWalkingLevel_Field[14].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_POWER_COST_SCHOOL_PCT] = timeWalkingLevel_Field[15].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_STAT_PCT] = timeWalkingLevel_Field[16].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_DAMAGESPELL] = timeWalkingLevel_Field[17].GetUInt32();
            _pctMap[TIMEWALKING_AURA_MOD_HEALING] = timeWalkingLevel_Field[18].GetUInt32();
            timeWalkingLevelsStatsList[timeWalkingLevel_Field[0].GetUInt32()*10000+timeWalkingLevel_Field[1].GetUInt32()*100+timeWalkingLevel_Field[2].GetUInt32()] = AzthLevelStat(timeWalkingLevel_Field[0].GetUInt32(), timeWalkingLevel_Field[1].GetUInt32(), timeWalkingLevel_Field[2].GetUInt32(), _pctMap);
        } while (timewalkingLevel_table->NextRow());

        sAzthLevelStatMgr->SetLevelStatList(timeWalkingLevelsStatsList);

        //-------------------------------------------------------------------
        //Loading achievement
        QueryResult azthAchievement_table = ExtraDatabase.PQuery("SELECT achievement,criteria,Points,category,parentCategory,difficulty,levelMax,levelMin,level,originalPoints,Name,Description,reward,rewardCount,killCredit,specialLevelReq,reqDimension FROM azth_achievements WHERE criteria != 0 ORDER BY achievement;");
        if (!azthAchievement_table)
        {
            sLog->outString(">> Loaded 0 achievements for TimeWalking. DB table `azth_achievements` is empty.\n");
            sLog->outString();
            return;
        }

        do
        {
            Field* azthAchievement_field = azthAchievement_table->Fetch();

            azthAchievementList[azthAchievement_field[1].GetUInt32()] = AzthAchievement(
                // achievement                                criteria                           Points                                 category                           parentCategory                    difficulty                        
                azthAchievement_field[0].GetUInt32(), azthAchievement_field[1].GetUInt32(), azthAchievement_field[2].GetUInt32(), azthAchievement_field[3].GetUInt32(), azthAchievement_field[4].GetUInt32(), azthAchievement_field[5].GetUInt32(),
                // levelMax                                 levelMin                                    level                              originalPoints                        Name                              Description
                azthAchievement_field[6].GetUInt32(), azthAchievement_field[7].GetUInt32(), azthAchievement_field[8].GetUInt32(), azthAchievement_field[9].GetUInt32(), azthAchievement_field[10].GetString(), azthAchievement_field[11].GetString(),
                //reward                                        rewardCount                             killcredit                      specialLevelReq                         reqDimension
                azthAchievement_field[12].GetUInt32(), azthAchievement_field[13].GetUInt32(), azthAchievement_field[14].GetUInt32(), azthAchievement_field[15].GetUInt32(), azthAchievement_field[16].GetUInt32());
        } while (azthAchievement_table->NextRow());

        sAzthAchievementMgr->achievementList=azthAchievementList;
    }
};

class TimeWalkingGossip : public CreatureScript
{
public:
    TimeWalkingGossip() : CreatureScript("TimeWalkingGossip") {}

    bool OnGossipHello(Player* player, Creature* creature) override
    {
		if (player->isUsingLfg())
		{
			player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_ACTIVELFG, creature->GetGUID());
			return true;
		}
		
        if (player->GetGroup())
        {
            player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_INGROUP, creature->GetGUID());
            return true;
        }

        if (!player->azthPlayer->isTimeWalking()) {
            if (player->getLevel()>=sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)) {
                //player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Tutte le fasi", GOSSIP_SENDER_MAIN, 5);
                player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "Livello specifico", GOSSIP_SENDER_MAIN, 6, "Imposta un livello", 0, true);
                //if (player->IsGameMaster()) {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Scaling automatico (beta)", GOSSIP_SENDER_MAIN, TIMEWALKING_LVL_AUTO+10000);
                //}
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "TimeWalking (Wotlk)", GOSSIP_SENDER_MAIN, 4);
            }
            player->ADD_GOSSIP_ITEM(0, "|TInterface/ICONS/INV_Misc_Coin_01:30|tFlex Mythic+ (Beta)", GOSSIP_SENDER_MAIN, 9); // we can't use another icon otherwise will be automatically selected on gossip hello
        } else {
            player->ADD_GOSSIP_ITEM(0, "Esci dalla modalità TimeWalking", GOSSIP_SENDER_MAIN, 7);
        }

        player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_MAIN, creature->GetGUID());

        return true;
    }
    
    bool _is_number(const std::string& s)
    {
        return !s.empty() && std::find_if(s.begin(), 
            s.end(), [](char c) { return !isdigit(c); }) == s.end();
    }

    void setTimeWalking(Player* player,uint32 level)
    {        
        player->azthPlayer->SetTimeWalkingLevel(level, true, true, false);
        player->CastSpell(player, 47292, TRIGGERED_FULL_MASK); // visual
    }

    bool OnGossipSelectCode(Player* player, Creature*  /*creature*/, uint32 sender, uint32 action, const char* code) override
    {
        player->PlayerTalkClass->ClearMenus();
        
        if (sender != GOSSIP_SENDER_MAIN)
            return false;


        if (action == 6 && _is_number(code))
        {
            int level = atoi(code);
            // we cannot set manually a level > 80
            // special level must be set by gossip menus
            if (level <= 0 || uint32(level) >= sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
            {
                return false;
            }

            setTimeWalking(player, level);
            player->PlayerTalkClass->SendCloseGossip();
        }

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32  /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == 4)
        {
            for (std::map<uint32, TwRaid>::iterator it = raidList.begin(); it != raidList.end(); it++)
            {
                if (it->second.GetBonus() == 1)
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "|cff00ff00|TInterface/ICONS/INV_Misc_Coin_01:30|t|rBonus: " + it->second.GetName()+"|r", GOSSIP_SENDER_MAIN, 10000+it->second.GetLevel());   
                }
            }
            player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_BONUS, creature->GetGUID());
        }

        if (action == 5)
        {
            std::vector<uint32> expList;

            for (std::map<uint32, TwRaid>::iterator it = raidList.begin(); it != raidList.end(); it++)
            {
                string exp;
                if (it->second.GetExp() == 1)
                {
                    exp = "Classic";
                }
                else if (it->second.GetExp() == 2)
                {
                    exp = "The Burning Crusade";
                }
                else
                {
                    exp = "Wrath of The Lich King";
                }

                if (std::find(expList.begin(), expList.end(), it->second.GetExp()) == expList.end()) {
                    expList.push_back(it->second.GetExp());
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, exp, GOSSIP_SENDER_MAIN, it->second.GetExp()); // go to phase menu
                }
            }
            player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_EXP, creature->GetGUID());
        }
        
        if (action == 9) {
            for (uint32 i=TIMEWALKING_LVL_VAS_START; i<=TIMEWALKING_LVL_VAS_LVL4; i++) {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Flex Mythic Level "+std::to_string(i-TIMEWALKING_LVL_VAS_START+1), GOSSIP_SENDER_MAIN, i+10000); 
            }
            
            player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_MAIN, creature->GetGUID());
        }

        if (action <= 3) //generate phase menu
        {
            std::vector<uint32> phaseList;

            for (std::map<uint32, TwRaid>::iterator it = raidList.begin(); it != raidList.end(); it++)
            {
                if (it->second.GetExp() == action)
                {
                    if (std::find(phaseList.begin(), phaseList.end(), it->second.GetPhase()) == phaseList.end()) {
                        phaseList.push_back(it->second.GetPhase());
                        std::stringstream s;
                        s << "Fase " << it->second.GetPhase();
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, s.str().c_str(), GOSSIP_SENDER_MAIN, 1000 + it->second.GetPhase());
                    }
                }
            }
            player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_PHASE, creature->GetGUID());
        }
        else if(action >= 1000 && action < 10000) //generate raid menu
        {
            for (std::map<uint32, TwRaid>::iterator it = raidList.begin(); it != raidList.end(); it++)
            {
                if (it->second.GetPhase() == action - 1000)
                {
                    player->ADD_GOSSIP_ITEM(0, it->second.GetName(), GOSSIP_SENDER_MAIN, it->second.GetLevel()+10000);
                }
            }
            player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_RAID, creature->GetGUID());
        }
        else if (action >= 10000) //apply level
        {
            uint32 level = action - 10000;

            if (player->azthPlayer->GetTimeWalkingLevel() == 0)
            {
                setTimeWalking(player, level);
                player->PlayerTalkClass->SendCloseGossip();
            }
            else
            {
                player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_ALREADYAPPLIED, creature->GetGUID());
            }
        }
        
        else if (action == 7)
        {
            setTimeWalking(player, 0 );
            player->PlayerTalkClass->SendCloseGossip();
        }

        return true;
    }
};

class timeWalkingPlayer : public PlayerScript
{
public:
    timeWalkingPlayer() : PlayerScript("timeWalkingPlayer") {}
    
    bool _autoscaling(Player *player, uint32 newZone = 0, uint32 newArea = 0) {
        player->azthPlayer->autoScalingPending=(player->IsInCombat() || !player->IsAlive() || !player->IsInWorld()) ? time(NULL)+2 /*2 seconds after */: 0;
        
        if (player->azthPlayer->autoScalingPending > 0)
            return false;

        uint32 posLvl = 0;
        if (newArea && newZone) {
            posLvl = sAzthUtils->getPositionLevel(false, player->GetMap(), newZone, newArea);
        } else {
            WorldLocation pos = WorldLocation(player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation()); 
            posLvl = sAzthUtils->getPositionLevel(false, player->GetMap(), pos); 
        }
        
        if (!posLvl)
            return false;

        if (player->getLevel() == posLvl)
            return false;
        
        player->azthPlayer->SetTimeWalkingLevel(TIMEWALKING_LVL_AUTO, true, true, false);

        return true;
    }
    
    void OnUpdateArea(Player* player, uint32 oldArea, uint32 newArea) override {
        if (!player)
            return;

        if (oldArea != newArea && player->azthPlayer->GetTimeWalkingLevel() == TIMEWALKING_LVL_AUTO) {
            AreaTableEntry const* area = sAreaTableStore.LookupEntry(newArea);
            if (!area)
                return;
            
            _autoscaling(player, area->zone, newArea);
        }

        sAzthUtils->updateTwLevel(player, player->GetGroup()); // needed for player stats
    }
    
    void OnBeforeUpdate(Player *player, uint32 /*p_time*/) override {
        if (!player)
            return;
        
        if (player->azthPlayer->GetTimeWalkingLevel() != TIMEWALKING_LVL_AUTO)
            return;
        
        if (player->azthPlayer->autoScalingPending > time(NULL))
            return;

        _autoscaling(player);
    }

    void OnCriteriaProgress(Player *player, AchievementCriteriaEntry const* criteria) override
    {
        if (!player || !player->IsInWorld())
            return;
        
		if (sAzthAchievementMgr->achievementList.find(criteria->ID) == sAzthAchievementMgr->achievementList.end())
			return;

        AzthAchievement achi = sAzthAchievementMgr->achievementList[criteria->ID];

        uint32 count = achi.GetRewardCount();
        uint32 reward = achi.GetReward();
        uint32 killCredit = achi.GetKillCredit();
        
        if (reward == AZTH_MARK_OF_AZEROTH && player->inRandomLfgDungeon()) {
            /* initialize random seed: */
            srand (time(NULL));
            /* generate secret number between 3 and 10: */
            uint8 moaBonus = 1 + rand() % static_cast<int>(4 - 1 + 1);

            if (player->GetGroup()) {
                std::list<Group::MemberSlot> memberSlots = player->GetGroup()->GetMemberSlots();
                for (std::list<Group::MemberSlot>::iterator membersIterator = memberSlots.begin(); membersIterator != memberSlots.end(); membersIterator++) {
                    if ((*membersIterator).guid == player->GetGUID()) {
                        uint8 role = (*membersIterator).roles;

                        if (role & lfg::PLAYER_ROLE_TANK || role & lfg::PLAYER_ROLE_HEALER) {
                            count += moaBonus;
                        }
                        break;
                    }
                }
            }
        }
        
        if (!count && !killCredit) {
            return;
        }
        
        /**
         *  CHECK REQUIREMENTS
         */
        if (achi.GetReqDimension() > 0 && achi.GetReqDimension() != player->azthPlayer->getCurrentDimensionByAura())
            return;

        uint32 level = player->azthPlayer->getPStatsLevel(false);
        
        // skip rewards if you don't have the required special level
        if (achi.GetSpecialLevelReq() > 0 && !sAzthUtils->isMythicCompLvl(achi.GetSpecialLevelReq(), level) && level != achi.GetSpecialLevelReq()) {
                return;
        }

        // if levelMin and Level max has been set, then the level must be in this range
        if (achi.GetLevelMin() && achi.GetLevelMax() && (level < achi.GetLevelMin() || level  > achi.GetLevelMax())) {
            return;
        }
        /*
         *  END 
         */

        // to achieve it you must be in range
        if (count > 0) {
            ItemTemplate const* _proto = sObjectMgr->GetItemTemplate(reward);
            if (!_proto)
                return;
            
            if (_proto->IsCurrencyToken()) {                
                if (player->GetMap()->IsDungeon() || player->GetMap()->IsRaid()) {
                    // bonus for who complete that achievement with a recommended level
                    if (level <= achi.GetLevel())
                        count *= 2;
                }

                player->AddItem(reward, count);
            } else {
                uint32 mailItems=0;

                SQLTransaction trans = CharacterDatabase.BeginTransaction();
                MailDraft* draft = new MailDraft(_proto->Name1, "");
                
                for (uint32 i = 0; i < count; i++)
                {
                    Item *item = NewItemOrBag(_proto);
                    
                    if (!item->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_ITEM), _proto->ItemId, player)) {
                        delete item;
                        return;
                    }

                    ItemPosCountVec off_dest;
                    if (mailItems == 0 && player->CanStoreItem(NULL_BAG, NULL_SLOT, off_dest, item, false) == EQUIP_ERR_OK)
                    {
                        player->StoreItem(off_dest, item, true);
                    } else {
                        draft->AddItem(item);
                        mailItems++;
                        
                        if ((i==count-1) || (mailItems > 0 && mailItems % 10 == 0)) {
                            draft->SendMailTo(trans, player, MailSender(player, MAIL_STATIONERY_GM), MAIL_CHECK_MASK_COPIED);
                            CharacterDatabase.CommitTransaction(trans);
                            
                            if (i<count-1) {
                                trans = CharacterDatabase.BeginTransaction();
                                draft = new MailDraft(_proto->Name1, "");
                            }
                        }
                    }
                }


                ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->getf(AZTH_LANG_TW_NEW_ITEM_OBTAINED, player, _proto->ItemId, _proto->Name1.c_str(), count));
                if (mailItems > 0) {
                    ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_BAG_FULL_SENT_TO_MAIL, player));
                }
            }
        }

        if (killCredit) {
            player->azthPlayer->ForceKilledMonsterCredit(killCredit, 0); // send credit 
        }

    }
    
    bool OnBeforeQuestComplete(Player *player, uint32 quest_id) override {
        uint32 guid,quest,sLevel,nLevel, instanceStart=0, questEnd, groupId;
        uint8 gSize;
        uint64 id=0;
        
        //Quest *questTmpl = sObjectMgr->GetQuestTemplate(quest_id);
        QuestStatusMap::iterator qsitr = player->getQuestStatusMap().find(quest_id);
        if (qsitr == player->getQuestStatusMap().end()) // should always be true in this moment
            return true;
        
        //if (player->GetGroup())
        //    groupId=player->GetGroup()->GetLowGUID();

        Map* map = player->FindMap();        
        if (!map->IsDungeon())
            return true;

        InstanceSave* is = sInstanceSaveMgr->PlayerGetInstanceSave(GUID_LOPART(player->GetGUID()), map->GetId(), player->GetDifficulty(map->IsRaid()));
        if (!is)
            return true;
        
        if (is->azthInstMgr->startTime == 0)
            return true;
        
        guid = player->GetGUIDLow();
        quest = quest_id;
        sLevel = player->azthPlayer->getPStatsLevel(false);
        nLevel = player->azthPlayer->getPStatsLevel(true);
        gSize = player->azthPlayer->getGroupSize();
        
        groupId = is->GetInstanceId();

        instanceStart = is->azthInstMgr->startTime;
    
        questEnd = static_cast<uint32>(time(NULL));
        
        id = MAKE_NEW_GUID(guid, groupId, instanceStart);
        
        CharacterDatabase.PExecute("INSERT INTO azth_quest_log (guid, groupId, quest, sLevel, nLevel, gSize, instanceStartTime, endTime) VALUES(%u,%u,%u,%u,%u,%u,%u,%u);", 
                                guid, id, quest, sLevel,nLevel,gSize,instanceStart,questEnd);

        return true;
    }

    void OnLoadFromDB(Player *player) override {
        QueryResult timewalkingCharactersActive_table = CharacterDatabase.PQuery(("SELECT id,level FROM azth_timewalking_characters_active WHERE id = %d;"), player->GetGUID());
        if (timewalkingCharactersActive_table) //if is in timewalking mode apply debuff
        {
            Field* timewalkingCharactersActive_field = timewalkingCharactersActive_table->Fetch();

            player->azthPlayer->SetTimeWalkingLevel(timewalkingCharactersActive_field[1].GetUInt32(), false, false , true);
        }
    }
    
    void OnLogin(Player *player) override {
        sAzthUtils->updateTwLevel(player, player->GetGroup()); // to fix level on instance that cannot be calculated OnLoadFromDB (too early)
        player->azthPlayer->prepareTwSpells(player->getLevel());
        sAzthUtils->setTwDefense(player, player->azthPlayer->isTimeWalking(true));
    }
    
    void OnLevelChanged(Player* player, uint8 /*oldlevel*/) override { 
        sAzthUtils->updateTwLevel(player, player->GetGroup());
    }
    
    void OnBeforeInitTalentForLevel(Player* player, uint8&  /*level*/, uint32& talentPointsForLevel) override
    {
        if (!sAzthUtils->isMythicLevel(player->azthPlayer->GetTimeWalkingLevel()) // redundant (?)
            && (player->azthPlayer->isTimeWalking(true) || player->azthPlayer->GetTimeWalkingLevel() == TIMEWALKING_LVL_AUTO) )
        {
            talentPointsForLevel = 71; // to avoid talent points reset after relog in timewalking
        }
    }
};


class achievement_timewalking_check : public AchievementCriteriaScript
{
    public:
        achievement_timewalking_check(char const* name) : AchievementCriteriaScript(name) {}

        //Should be deprecated since canCompleteCriteria used in official AchievementMgr.cpp check is enough
        //however you can attach this script on achievement_criteria_data if needed
        bool OnCheck(Player*  player, Unit* /*target*/, uint32 criteria_id) override
        {
            AchievementCriteriaEntry const* criteria = sAchievementCriteriaStore.LookupEntry(criteria_id);
            return player->azthPlayer->canCompleteCriteria(criteria);  
        }
};

class global_timewalking : public GlobalScript {
    public:
        global_timewalking() : GlobalScript("global_timewalking_script") { }
        
        void OnItemRoll(Player const* player, LootStoreItem const *item, float &chance, Loot &loot, LootStore const& store) override {
            // this check assume that sAzthUtils->isEligibleForBonusByArea(player) has been already checked
            if (!loot.azthSecondRound)
                return;
            
            if ((loot.quest_items.size() + loot.items.size()) >= MAX_NR_LOOT_ITEMS) {
                chance = 0;
                return;
            }
            
            if (chance >= 100.0f || chance <= 0)
                return;

            if (!player->GetMap()->IsDungeon() && !player->GetMap()->IsRaid())
                return;

            if (item->mincountOrRef >= 0) {
                ItemTemplate const* i = sObjectMgr->GetItemTemplate (item->itemid);

                if (i && i->Quality < ITEM_QUALITY_RARE)
                    return;
            }
            
            if (&store != &LootTemplates_Gameobject && &store != &LootTemplates_Creature)
                return;

            if (chance < 20.f)
                chance += 20.f;
            //else
            //    chance *= 2;
        }
        
        void OnInitializeLockedDungeons(Player* player, uint8& /*level*/, uint32& lockData, lfg::LFGDungeonData const* dungeon) override {
            switch(lockData){
                case lfg::LFG_LOCKSTATUS_MISSING_ACHIEVEMENT:
                case lfg::LFG_LOCKSTATUS_QUEST_NOT_COMPLETED:
                case lfg::LFG_LOCKSTATUS_MISSING_ITEM:
                    Player* leader = nullptr;
                    uint64 leaderGuid = player->GetGroup() ? player->GetGroup()->GetLeaderGUID() : player->GetGUID();
                    if (leaderGuid != player->GetGUID())
                        leader = HashMapHolder<Player>::Find(leaderGuid);
                        
                    if (!leader || !leader->IsInWorld())
                        leader=player;

                    if (sWorld && leader->IsInWorld() &&
                    leader->azthPlayer && leader->azthPlayer->isTimeWalking() && dungeon && dungeon->minlevel<=70) {
                        lockData = 0;
                    }
                break;
            }
        }
};

void AddSC_TimeWalking()
{
    new loadTimeWalkingRaid();
    new global_timewalking();
    new TimeWalkingGossip();
    new timeWalkingPlayer();
    new achievement_timewalking_check("achievement_timewalking_check");
    new TWVasScript();
}
