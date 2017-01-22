#include "ScriptMgr.h"
#include "Player.h"
#include "Common.h"
#include "raid.h"
#include "Creature.h"
#include "AzthPlayer.h"
#include "AzthLevelStat.h"

std::map<uint32, raid> raidList;
std::map<uint32, AzthLevelStat> timeWalkingLevelsStatsList;

enum npc_timewalking_enum
{
    TIMEWALKING_GOSSIP_NPC_TEXT_MAIN = 50100,
    TIMEWALKING_GOSSIP_NPC_TEXT_BONUS = 50101,
    TIMEWALKING_GOSSIP_NPC_TEXT_EXP = 50102,
    TIMEWALKING_GOSSIP_NPC_TEXT_PHASE = 50103,
    TIMEWALKING_GOSSIP_NPC_TEXT_RAID = 50104,
    TIMEWALKING_GOSSIP_NPC_TEXT_ALREADYAPPLIED = 50105,
};

class loadTimeWalkingRaid : public WorldScript
{
public:
    loadTimeWalkingRaid() : WorldScript("loadTimeWalkingRaid") {}
    void OnStartup()
    {
        QueryResult timewalking_table = ExtraDatabase.PQuery("SELECT id,name,exp,phase,level,bonus FROM timewalking ORDER BY exp, phase, level, name;"); //author, title, music id and genre from DB
        if (!timewalking_table)
        {
            sLog->outString(">> Loaded 0 raids for TimeWalking. DB table `timewalking` is empty.\n");
            sLog->outString();
            return;
        }
        Field* timeWalking_Field = timewalking_table->Fetch();

        do
        {
            raidList[timeWalking_Field[0].GetUInt32()] = raid(timeWalking_Field[0].GetUInt32(), timeWalking_Field[1].GetString(), timeWalking_Field[2].GetUInt32(), timeWalking_Field[3].GetUInt32(), timeWalking_Field[4].GetUInt32(), timeWalking_Field[5].GetUInt32());
        } while (timewalking_table->NextRow());
    
        
        
        QueryResult timewalkingLevel_table = ExtraDatabase.PQuery("SELECT level,race,class,health,resistance,healing,damage,power_cost,all_stat,mana,crit_chance,miss_chance,dodge_chance,parry_chance,block_chance FROM timewalking_levels ORDER BY level;");
        if (!timewalkingLevel_table)
        {
            sLog->outString(">> Loaded 0 levels for TimeWalking. DB table `timewalking_levels` is empty.\n");
            sLog->outString();
            return;
        }

        Field* timeWalkingLevel_Field = timewalkingLevel_table->Fetch();

        do
        {
            timeWalkingLevelsStatsList[timeWalkingLevel_Field[0].GetUInt32()*1000+timeWalkingLevel_Field[1].GetUInt32()*10+timeWalkingLevel_Field[2].GetUInt32()] = AzthLevelStat(timeWalkingLevel_Field[0].GetUInt32(), timeWalkingLevel_Field[1].GetUInt32(), timeWalkingLevel_Field[2].GetUInt32(), timeWalkingLevel_Field[3].GetUInt32(), timeWalkingLevel_Field[4].GetUInt32(), timeWalkingLevel_Field[5].GetUInt32(), timeWalkingLevel_Field[6].GetUInt32(), timeWalkingLevel_Field[7].GetUInt32(), timeWalkingLevel_Field[8].GetUInt32(), timeWalkingLevel_Field[9].GetFloat(), timeWalkingLevel_Field[10].GetUInt32(), timeWalkingLevel_Field[11].GetUInt32(), timeWalkingLevel_Field[12].GetUInt32(), timeWalkingLevel_Field[13].GetUInt32(), timeWalkingLevel_Field[14].GetUInt32());
        } while (timewalkingLevel_table->NextRow());

        sAzthLevelStat->SetLevelStatList(timeWalkingLevelsStatsList);
    }
};

class TimeWalkingGossip : public CreatureScript
{
public:
    TimeWalkingGossip() : CreatureScript("TimeWalkingGossip") {}

    bool OnGossipHello(Player* player, Creature* creature)
    {
        player->ADD_GOSSIP_ITEM(0, "Raid con bonus", GOSSIP_SENDER_MAIN, 4);
        player->ADD_GOSSIP_ITEM(0, "Raid standard", GOSSIP_SENDER_MAIN, 5);
        player->ADD_GOSSIP_ITEM_EXTENDED(0, "Livello specifico", GOSSIP_SENDER_MAIN, 6, "Imposta un livello", 0, true);
        player->ADD_GOSSIP_ITEM(0, "Esci dalla modalità TimeWalking", GOSSIP_SENDER_MAIN, 7);
        player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_MAIN, creature->GetGUID());
        return true;
    }

    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == 6)
        {
            int level = atoi(code);
            if (level < 0 || level > 80)
            {
                return false;
            }

            player->azthPlayer->SetTimeWalkingLevel(level);
            player->PlayerTalkClass->SendCloseGossip();
        }

    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == 4)
        {
            for (std::map<uint32, raid>::iterator it = raidList.begin(); it != raidList.end(); it++)
            {
                if (it->second.GetBonus() == 1)
                {
                    player->ADD_GOSSIP_ITEM(0, "|cFFf91616Bonus: " + it->second.GetName()+"|r", GOSSIP_SENDER_MAIN, 10000+it->second.GetLevel());   
                }
                player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_BONUS, creature->GetGUID());
            }
        }

        if (action == 5)
        {
            std::vector<uint32> expList;

            for (std::map<uint32, raid>::iterator it = raidList.begin(); it != raidList.end(); it++)
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
                    player->ADD_GOSSIP_ITEM(0, exp, GOSSIP_SENDER_MAIN, it->second.GetExp()); // go to phase menu
                }
            }
            player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_EXP, creature->GetGUID());
        }

        if (action <= 3) //generate phase menu
        {
            std::vector<uint32> phaseList;

            for (std::map<uint32, raid>::iterator it = raidList.begin(); it != raidList.end(); it++)
            {
                if (it->second.GetExp() == action)
                {
                    if (std::find(phaseList.begin(), phaseList.end(), it->second.GetPhase()) == phaseList.end()) {
                        phaseList.push_back(it->second.GetPhase());
                        std::stringstream s;
                        s << "Fase " << it->second.GetPhase();
                        player->ADD_GOSSIP_ITEM(0, s.str().c_str(), GOSSIP_SENDER_MAIN, 1000 + it->second.GetPhase());
                    }
                    player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_PHASE, creature->GetGUID());
                }
            }
        }
        else if(action >= 1000 && action < 10000) //generate raid menu
        {
            for (std::map<uint32, raid>::iterator it = raidList.begin(); it != raidList.end(); it++)
            {
                if (it->second.GetPhase() == action - 1000)
                {
                    player->ADD_GOSSIP_ITEM(0, it->second.GetName(), GOSSIP_SENDER_MAIN, it->second.GetLevel()+10000);
                    player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_RAID, creature->GetGUID());
                }
            }
        }
        else if (action >= 10000) //apply level
        {
            uint32 level = action - 10000;
            if (player->azthPlayer->GetTimeWalkingLevel() == NULL)
            {
                player->azthPlayer->SetTimeWalkingLevel(level);
                player->PlayerTalkClass->SendCloseGossip();
            }
            else
            {
                player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_ALREADYAPPLIED, creature->GetGUID());
            }
        }
        
        else if (action == 7)
        {
            player->azthPlayer->SetTimeWalkingLevel(NULL);
            player->PlayerTalkClass->SendCloseGossip();
        }

        return true;
    }
};


class modAttackMelleeStats : public UnitScript
{
public:
    modAttackMelleeStats() : UnitScript("modAttackMelleeStats") {}
    void OnBeforeRollMeleeOutcomeAgainst(const Unit* attacker, const Unit* victim, WeaponAttackType attType, int32 &crit_chance, int32 &miss_chance, int32 &dodge_chance, int32 &parry_chance, int32 &block_chance)
    {
        if (victim->isType(TYPEMASK_PLAYER))
        {
            if (victim->ToPlayer()->azthPlayer->GetTimeWalkingLevel() != NULL)
            {
                map<uint32, AzthLevelStat> levelStatList = sAzthLevelStat->GetLevelStatList();
                uint32 race = victim->ToPlayer()->getRace();
                uint32 Class = victim->ToPlayer()->getClass();
                for (std::map<uint32, AzthLevelStat>::iterator it = levelStatList.begin(); it != levelStatList.end(); it++)
                {
                    if (it->second.GetRace() == race && it->second.GetClass() == Class)
                    {
                        if (it->second.GetLevel() == victim->ToPlayer()->azthPlayer->GetTimeWalkingLevel())
                        {
                            AzthLevelStat stats = it->second;
                            crit_chance = stats.GetCritChance();
                            miss_chance = stats.GetMissChance();
                            dodge_chance = stats.GetDodgeChance();
                            parry_chance = stats.GetParryChance();
                            block_chance = stats.GetBlockChance();
                        }
                    }
                }
            }

        }
    }
};

class timeWalkingPlayer : public PlayerScript
{
public:
    timeWalkingPlayer() : PlayerScript("timeWalkingPlayer") {}
    void OnLogin(Player* player) 
    {
        QueryResult timewalkingCharactersActive_table = ExtraDatabase.PQuery(("SELECT id,level FROM timewalking_characters_active WHERE id = %d;"), player->GetGUID());
        //clean player to avoid problems
        player->azthPlayer->SetTimeWalkingLevel(NULL);
        if (timewalkingCharactersActive_table) //if is in timewalking mode apply debuff
        {
            Field* timewalkingCharactersActive_field = timewalkingCharactersActive_table->Fetch();

            player->azthPlayer->SetTimeWalkingLevel(timewalkingCharactersActive_field[1].GetUInt32());
        }
    }
    
    /*void OnAfterUpdateMaxPower(Player* player, Powers& power, float& value)
    {
        if (player->azthPlayer->GetTimeWalkingLevel() != NULL)
        {
            if (power == POWER_MANA)
            {
                value = 0.0f;
            }
        }
    }*/
};

void AddSC_TimeWalking()
{
    new loadTimeWalkingRaid();
    new TimeWalkingGossip();
    new timeWalkingPlayer();
    new modAttackMelleeStats();
}