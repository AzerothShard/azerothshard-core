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
	TIMEWALKING_GOSSIP_NPC_TEXT_ACTIVELFG = 50106
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

        if (!player->azthPlayer->isTimeWalking()) {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "|TInterface/ICONS/INV_Misc_Coin_01:30|t Fase Attuale ( Bonus )", GOSSIP_SENDER_MAIN, 4);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Tutte le fasi", GOSSIP_SENDER_MAIN, 5);
            player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "Livello specifico", GOSSIP_SENDER_MAIN, 6, "Imposta un livello", 0, true);
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
        if (level == sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)) // we should skip level 80 and use specials (>300) if needed
            return;
        
        uint32 iLvl=player->azthPlayer->getTwItemLevel(level);
        if (iLvl && !player->azthPlayer->checkItems(iLvl)) {
            ChatHandler(player->GetSession()).PSendSysMessage("Questa modalità ha l'item level massimo: %u",iLvl);
            return;
        }
        
        player->azthPlayer->SetTimeWalkingLevel(level);

        sAzthUtils->removeTimewalkingAura(player);
        if (player->GetPet() != NULL) {
            sAzthUtils->removeTimewalkingAura(player->GetPet());
        }

        player->SaveToDB(false, false);
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
            if (level < 0 || uint32(level) >= sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
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
            setTimeWalking(player, 0);
            player->PlayerTalkClass->SendCloseGossip();
        }

        return true;
    }
};

class timeWalkingPlayer : public PlayerScript
{
public:
    timeWalkingPlayer() : PlayerScript("timeWalkingPlayer") {}

    void OnCriteriaProgress(Player *player, AchievementCriteriaEntry const* criteria) override
    {
		if (sAzthAchievementMgr->achievementList.find(criteria->ID) == sAzthAchievementMgr->achievementList.end())
			return;

        AzthAchievement achi = sAzthAchievementMgr->achievementList[criteria->ID];

        uint32 count = achi.GetRewardCount();
        uint32 reward = achi.GetReward();
        uint32 killCredit = achi.GetKillCredit();
        
        if (!count && !killCredit) {
            return;
        }
        
        /**
         *  CHECK REQUIREMENTS
         */
        if (achi.GetReqDimension() > 0 && achi.GetReqDimension() != player->azthPlayer->getCurrentDimensionByAura())
            return;

        uint32 groupLevel= player->azthPlayer->getGroupLevel();
        uint32 specialGroupLevel = player->azthPlayer->getGroupLevel(false);
        uint32 level = groupLevel > 0 ? groupLevel : player->getLevel();
        uint32 specialLevel = specialGroupLevel > 0 ? specialGroupLevel : player->azthPlayer->GetTimeWalkingLevel();
        
        // skip rewards if you don't have the required special level
        if (achi.GetSpecialLevelReq() > 0 && 
            (!player->azthPlayer->isTimeWalking() || specialLevel != achi.GetSpecialLevelReq()) ) {
            return;
        }
        
        if (level < achi.GetLevelMin() || level  > achi.GetLevelMax() ) {
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


            bool hasBonus = false;
            RaidList rList = sAzthRaid->GetRaidList();
            for (RaidList::iterator itr = rList.begin(); itr != rList.end(); itr++) {
                if ((*itr).second.GetCriteria() == criteria->ID && (*itr).second.hasBonus()) {
                    hasBonus = true;
                    break;
                }
            }
            
            if (_proto->IsCurrencyToken()) {
                // if bonus is active then you'll get x2 tokens
                // moreover if you have the level <= of suggested
                // then you'll get x6 tokens instead
                if (hasBonus) {
                    count *= 2;

                    if (level <= achi.GetLevel())
                        count *= 3;
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


                ChatHandler(player->GetSession()).PSendSysMessage("Complimenti! hai ottenuto |cffff0000|Hitem:%u::::::::::::|h[%s]|h|r x%d|r ", _proto->ItemId, _proto->Name1.c_str(), count);
                if (mailItems > 0) {
                    ChatHandler(player->GetSession()).PSendSysMessage("Hai le borse piene! Controlla la mailbox!");
                }
            }
        }

        if (killCredit) {
            player->azthPlayer->ForceKilledMonsterCredit(killCredit, 0); // send credit 
        }

    }
    
    void OnLoadFromDB(Player *player) override {
        QueryResult timewalkingCharactersActive_table = CharacterDatabase.PQuery(("SELECT id,level FROM azth_timewalking_characters_active WHERE id = %d;"), player->GetGUID());
        if (timewalkingCharactersActive_table) //if is in timewalking mode apply debuff
        {
            Field* timewalkingCharactersActive_field = timewalkingCharactersActive_table->Fetch();

            player->azthPlayer->SetTimeWalkingLevel(timewalkingCharactersActive_field[1].GetUInt32());
        }
    }
    
    void OnLogin(Player *player) override {
        sAzthUtils->updateTwLevel(player); // to fix level on instance that cannot be calculated OnLoadFromDB (too early)
    }

    
    void OnBeforeInitTalentForLevel(Player* player, uint8&  /*level*/, uint32& talentPointsForLevel) override
    {
        if (player->azthPlayer->isTimeWalking())
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

void AddSC_TimeWalking()
{
    new loadTimeWalkingRaid();
    new TimeWalkingGossip();
    new timeWalkingPlayer();
    new achievement_timewalking_check("achievement_timewalking_check");
}
