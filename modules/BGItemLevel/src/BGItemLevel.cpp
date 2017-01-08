#include "ScriptMgr.h"
#include "Player.h"
#include "Item.h"
#include "ItemPrototype.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "QueryResult.h"
#include "ArenaSeason.h"
#include "Common.h"
#include "ArenaTeam.h"

uint32 maxItemLevel;

class loadSeason : public WorldScript
{
public:
  loadSeason() : WorldScript("loadSeason") {}
  void OnStartup()
  {
    QueryResult result = WorldDatabase.PQuery("SELECT * FROM season ORDER BY startingFrom;");
    QueryResult getLastDate = CharacterDatabase.PQuery("SELECT * FROM worldstates WHERE entry = 100000;");

    if (!result)
    {
        sLog->outString(">> Loaded 0 season. DB table `season` is empty.");
        sLog->outString();
        return;
    }

    Field* season_table = result->Fetch();
    Field* arena_timestamp_table = getLastDate->Fetch();

    //get last date of a system change
    time_t lastChange = time_t(arena_timestamp_table[1].GetUInt32());
    struct tm * lastChangeDateConverted = localtime(&lastChange);
    uint32 lastChangeDay = lastChangeDateConverted->tm_mday;


    //get status of the system: enabled or disabled
    istringstream(arena_timestamp_table[2].GetString()) >> enable;


    bool actualSeasonFound = false;

    //ACTUAL TIME FROM CORE
    time_t t = time(0);   
    struct tm * now = localtime(&t);
    uint32 month = now->tm_mon + 1;
    uint32 day = now->tm_mday;
    uint32 seconds = (now->tm_hour * 60 * 60) + (now->tm_min * 60) + now->tm_sec;
    uint32 date = (month * 31 * 24 * 60 * 60) + (day * 24 * 60 * 60) + seconds;


    //check if today is friday and is not the same friday of today
    if (now->tm_wday == 5 && day != lastChangeDay)
    {
            if (enable)
            {
                ArenaSeasonSystemEnabled = false;
                QueryResult setModeDisabled = CharacterDatabase.PQuery("UPDATE worldstates SET comment=0 WHERE entry=100000;"); //set arena season to disabled
            }
            else
            {
                sASeasonMgr->SetEnabled(true);
                QueryResult setModeEnabled = CharacterDatabase.PQuery("UPDATE worldstates SET comment=1 WHERE entry=100000;"); //set arena season to enabled
            }
            QueryResult setLastDate = CharacterDatabase.PQuery("UPDATE worldstates SET value=%u WHERE entry=100000;", t); //set new timestamp
    }

    //TIME FROM DB
    time_t startingDate;
    time_t endDate;

    do
    {
        //retrieve timestamp from db
        startingDate = time_t(season_table[1].GetUInt32());
        endDate = time_t(season_table[2].GetUInt32());

        //convert start timestamp to something that can be read
        struct tm * startingDateConverted = localtime(&startingDate);
        
        //assign value to every converted value from start timestamp
        uint32 dbMonthStart = startingDateConverted->tm_mon + 1;
        uint32 dbDayStart = startingDateConverted->tm_mday;
        uint32 dbSecondsStart = (startingDateConverted->tm_hour * 60 * 60) + (startingDateConverted->tm_min * 60) + startingDateConverted->tm_sec;
        
        uint32 dbStartDate = (dbMonthStart * 31 * 24 * 60 * 60) + (dbDayStart * 24 * 60 * 60) + dbSecondsStart;

        //convert end timestamp to something that can be read
        struct tm * EndDateConverted = localtime(&endDate);

        //assign value to every converted value from end timestamp
        uint32 dbMonthEnd = EndDateConverted->tm_mon + 1;
        uint32 dbDayEnd = EndDateConverted->tm_mday;
        uint32 dbSecondsEnd = (EndDateConverted->tm_hour * 60 * 60) + (EndDateConverted->tm_min * 60) + EndDateConverted->tm_sec;

        uint32 dbEndDate = (dbMonthEnd * 31 * 24 * 60 * 60) + (dbDayEnd * 24 * 60 * 60) + dbSecondsEnd;

        if (date > dbStartDate && date < dbEndDate)
        {
            sASeasonMgr->SetItemLevel(season_table[0].GetUInt32());
            sASeasonMgr->SetStartingDate(time_t(season_table[1].GetUInt32()));
            sASeasonMgr->SetEndDate(time_t((season_table[2].GetUInt32())));
            actualSeasonFound = true;
        }
        
    } while (!actualSeasonFound && result->NextRow());

    //prevent crashes if any season found
    if (sASeasonMgr->GetItemLevel() == 0)
    {
        sLog->outString(">> No correspondent season found. Check DB table `season`.\n");
        sLog->outString();
        sASeasonMgr->SetEnabled(false);
        return; 
    }

    maxItemLevel = sASeasonMgr->GetItemLevel();

    sLog->outString(">> Season script for PVP loaded, actual item level: %u\n", sASeasonMgr->GetItemLevel());
    sLog->outString();
  }
};

class checkItemLevel : public PlayerScript
{
public:
  checkItemLevel() : PlayerScript("checkItemLevel") {}

  //Check not compatible items
  void checkPlayerItem(Player* player, Battleground* battleground, bool inBattleground)
  {

    if (!sASeasonMgr->IsEnabled())
    {
        return; //SYSTEM DISABLED
    }

    if (player->IsGameMaster())
    {
        return;
    }

    uint32 INVENTORY_END = 18;
    uint32 counter = 0;
    string incompatibleItems[18];
    bool incompatible = false;

    for (uint32 INVENTORY_INDEX = 0; INVENTORY_INDEX < INVENTORY_END; INVENTORY_INDEX++)
    {
      Item* itemToCheck = player->GetItemByPos(INVENTORY_SLOT_BAG_0, INVENTORY_INDEX);
      if (itemToCheck != nullptr)
      {
        if (itemToCheck->GetTemplate()->ItemLevel > maxItemLevel)
        {
          incompatibleItems[counter] = itemToCheck->GetTemplate()->Name1;
          counter++;
          incompatible = true;
        }
      }
    }
    if (incompatible)
    {
      if (!inBattleground)
      {
        for (uint32 qslot = 0; qslot < PLAYER_MAX_BATTLEGROUND_QUEUES; ++qslot)
          if (BattlegroundQueueTypeId q = player->GetBattlegroundQueueTypeId(qslot))
          {
            BattlegroundQueue& queue = sBattlegroundMgr->GetBattlegroundQueue(q);
            queue.RemovePlayer(player->GetGUID(), q, qslot);
            player->RemoveBattlegroundQueueId(q);
          }
      }
      else
      {
        player->LeaveBattleground(); //works also with arena
      }
      for (uint32 i = 0; i < counter; i++)
      {
        ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000%s|r ha un livello troppo alto! Rimuovilo per poter giocare questa season.", incompatibleItems[i].c_str());
      }
      ChatHandler(player->GetSession()).PSendSysMessage("L'attuale Season ha livello massimo |cffff0000%d|r", maxItemLevel);
    }
  }

  //Check if a player join (queue) battleground with not compatible items
  void OnPlayerJoinBG(Player* player, Battleground* battleground)
  {
    checkPlayerItem(player, battleground, false);
  }

  //Check if a player join (queue) arena with not compatible items
  void OnPlayerJoinArena(Player* player, Battleground* battleground)
  {
    checkPlayerItem(player, battleground, false);
  }

  //Check if a player is just entered in battleground/arena with not compatible items
  void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea)
  {
    if (player->InBattleground() || player->InArena())
    {
      Battleground* battleground = player->GetBattleground();
      checkPlayerItem(player, battleground, true);
    }
  }

  //Check if a player equip not compatible item during battleground/arena
  void OnEquip(Player* player, Item* item, uint8 bag, uint8 slot, bool update)
  {
    if (player->InBattleground() || player->InArena())
    {
      Battleground* battleground = player->GetBattleground();
      checkPlayerItem(player, battleground, true);
      player->AddAura(26913, player);
    }
  }
};


class arenaPointsModifier : public GlobalScript
{
public:
    arenaPointsModifier() : GlobalScript("arenaPointsModifier") {}

    
    void OnBeforeUpdateArenaPoints(ArenaTeam* at, std::map<uint32, uint32> &ap)
    {
        for (ArenaTeam::MemberList::const_iterator itr = at->m_membersBegin(); itr != at->m_membersEnd(); ++itr)
        {
            uint32 points = ap[GUID_LOPART(itr->Guid)];
            uint8 minGames = 10;

            if (points == 0)
                continue;

            // assume you've done at least a win to get the basic bonus
            uint32 weekWins = at->GetStats().WeekWins ? at->GetStats().WeekWins : 1;

            float modifier = float(at->GetStats().WeekWins * 100 / minGames) / 100;

            if (modifier < 0.1)
                modifier = 0.1; // 10% as min value
            else if (modifier > 2) // max value
                modifier = 2;

            // sorry about multiple floating, just to be sure :P
            ap[GUID_LOPART(itr->Guid)] = float(points * modifier);
        }
    }
};

void AddSC_BGItemLevel()
{
  new checkItemLevel();
  new loadSeason();
  new arenaPointsModifier();
}