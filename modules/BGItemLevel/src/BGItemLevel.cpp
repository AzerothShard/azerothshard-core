#include "ScriptMgr.h"
#include "Player.h"
#include "Item.h"
#include "ItemPrototype.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "QueryResult.h"

uint32 maxItemLevel;

class loadSeason : public WorldScript
{
public:
  loadSeason() : WorldScript("loadSeason") {}
  void OnAfterConfigLoad(bool reload)
  {
    QueryResult getItemLevel = WorldDatabase.PQuery("SELECT itemLevel FROM season");
    Field* fields = getItemLevel->Fetch();
    maxItemLevel = fields[0].GetUInt32();
    printf_s("itemLevel for BG Loaded: %d", maxItemLevel);

  }
};

class checkItemLevel : public PlayerScript
{
public:
  checkItemLevel() : PlayerScript("checkItemLevel") {}

  //Check not compatible items
  void checkPlayerItem(Player* player, Battleground* battleground, bool inBattleground)
  {
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

  //Check if a group join (queue) arena with not compatible items
  void OnPlayerJoinArena(Player* player, Battleground* battleground)
  {
    checkPlayerItem(player, battleground, false);
  }

  //Check if a player is just entered in battleground/arena with not compatible items
  void OnMapChanged(Player* player)
  {
    if (player->InBattleground() || player->InArena())
    {
      Battleground* battleground = player->GetBattleground();
      checkPlayerItem(player, battleground, true);
    }
  }

  //Check if a player and equip not compatible item during battleground/arena
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

void AddSC_BGItemLevel()
{
  new checkItemLevel();
  new loadSeason();
}