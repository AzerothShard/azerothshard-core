#include "AzthSmartStone.h"
#include "Chat.h"
#include "Common.h"
#include "Define.h"
#include "GossipDef.h"
#include "Item.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Spell.h"
#include "WorldPacket.h"
#include "WorldSession.h"

uint32 parent = 1;

class azth_smart_stone : public ItemScript {
public:
  azth_smart_stone() : ItemScript("azth_smart_stone") {}

  bool OnUse(Player *player, Item *item, SpellCastTargets const &targets) {
    player->PlayerTalkClass->ClearMenus();

    player->ADD_GOSSIP_ITEM(0, "Benvenuto nella tua SmartStone!",
                            GOSSIP_SENDER_MAIN, 99999);

    if (parent == 1) {
      // black market teleport id 1
      SmartStoneCommand teleport = sSmartStone->getCommandById(1);
      player->ADD_GOSSIP_ITEM(teleport.icon, teleport.text, GOSSIP_SENDER_MAIN,
                              teleport.id);

      // menu character (rename, change faction, etc) id 4
      SmartStoneCommand characterMenu = sSmartStone->getCommandById(4);
      player->ADD_GOSSIP_ITEM(characterMenu.icon, characterMenu.text,
                              GOSSIP_SENDER_MAIN, characterMenu.id);

      // menu passive bonus id 9
      SmartStoneCommand passiveMenu = sSmartStone->getCommandById(9);
      player->ADD_GOSSIP_ITEM(passiveMenu.icon, passiveMenu.text,
                              GOSSIP_SENDER_MAIN, passiveMenu.id);
    }

    std::vector<SmartStonePlayerCommand> playerCommands =
        player->azthPlayer->getSmartStoneCommands();
    int n = playerCommands.size();

    for (int i = 0; i < n; i++) {
      SmartStoneCommand command =
          sSmartStone->getCommandById(playerCommands[i].id);

      // if expired or no charges
      if ((playerCommands[i].duration <= time(NULL) &&
           playerCommands[i].duration != 0) ||
          playerCommands[i].charges == 0) {
        player->azthPlayer->removeSmartStoneCommand(playerCommands[i], true);
        continue;
      }

      std::string text = command.text;

      if (playerCommands[i].charges != -1)
        text = text + " (" + std::to_string(playerCommands[i].charges) + ")";

      if (playerCommands[i].duration != 0) {
        uint64 timeDiff = playerCommands[i].duration - time(NULL);
        uint64 seconds = timeDiff % 60;
        uint64 minutes = floor(timeDiff / 60);
        uint64 hours = floor(timeDiff / 3600);
        uint64 days = floor(timeDiff / 3600 / 24);
        if (days >= 1) {
          text = text + " (" + std::to_string(days) + " giorni)";
        } else {
          text = text + " (" + std::to_string(hours) + ":" +
                 std::to_string(minutes) + ":" + std::to_string(seconds) + ")";
        }
      }

      if (!command.id == 0 && command.parent_menu == parent)
        player->ADD_GOSSIP_ITEM(command.icon, text, GOSSIP_SENDER_MAIN,
                                command.id);
    }

    // acquista app

    if (parent == 1)
      player->ADD_GOSSIP_ITEM(
          0, "|TInterface/ICONS/INV_Misc_Coin_03:30|t Azeroth Store",
          GOSSIP_SENDER_MAIN, 2000);

    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, item->GetGUID());

    parent = 1;
    return true;
  }

  void OnGossipSelect(Player *player, Item *item, uint32 sender,
                      uint32 action) override {
    player->PlayerTalkClass->ClearMenus();

    SmartStoneCommand selectedCommand = sSmartStone->getCommandById(action);

    // scripted action
    if (selectedCommand.type == DO_SCRIPTED_ACTION ||
        action == 2000) // azeroth store
    {
      if (selectedCommand.charges > 0) {
        player->azthPlayer->decreaseSmartStoneCommandCharges(
            selectedCommand.id);
      }
      switch (action) {
      case 2000: // store
        sSmartStone->SmartStoneSendListInventory(player->GetSession());
        break;

      case 1: // black market teleport
      {
        if (!player->IsInCombat())
          player->TeleportTo(1, 4818.27f, -1971.3f, 1069.75f, 0.174f, 0);
      } break;

      case 2: // change faction
      {
        player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
        ChatHandler(player->GetSession())
            .SendSysMessage("Rilogga per cambiare fazione!");
      } break;

      case 3: // rename
      {
        player->SetAtLoginFlag(AT_LOGIN_RENAME);
        ChatHandler(player->GetSession()).SendSysMessage("Rilogga per cambiare "
                                                         "nome! Attento, gli "
                                                         "addon sono settati "
                                                         "in base al nome!");
      } break;

      case 5: // change race
      {
        player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
        ChatHandler(player->GetSession())
            .SendSysMessage("Rilogga per cambiare razza!");
      } break;

      case 6: // jukebox
        player->SummonCreature(300205, player->GetPositionX(),
                               player->GetPositionY(), player->GetPositionZ(),
                               player->GetOrientation(),
                               TEMPSUMMON_TIMED_DESPAWN, 600 * 1000, 0);
        break;

      case 99999:
        break;
      default:
        sLog->outError("Smartstone: unhandled command! ID: %u, player GUID: %u",
                       action, player->GetGUID());
        break;
      }
      player->CLOSE_GOSSIP_MENU();
      // return;
    }

    // open child
    if (selectedCommand.type == OPEN_CHILD) {
      parent = selectedCommand.action;
      player->CLOSE_GOSSIP_MENU();
      OnUse(player, item, SpellCastTargets());
      /*
      player->PlayerTalkClass->ClearMenus();

      std::vector<SmartStoneCommand> playerCommands =
      player->azthPlayer->getSmartStoneCommands();
      int n = playerCommands.size();


      for (int i = 0; i < n; i++)
      {
          SmartStoneCommand command = playerCommands[i];
          if (!command.id == 0 && command.parent_menu == selectedCommand.action)
          {
              player->ADD_GOSSIP_ITEM(command.icon, command.text,
      GOSSIP_SENDER_MAIN, command.id);
          }
      }

      if (selectedCommand.action == 1)
          player->ADD_GOSSIP_ITEM(0, "|TInterface/ICONS/INV_Misc_Coin_03:30|t
      Azeroth Store", GOSSIP_SENDER_MAIN, 2000);

      player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
      */

      // return;
    }
  }
};

void SmartStone::loadCommands() {
  // initialize count and array
  uint32 count = 0;
  // sHearthstoneMode->hsAchievementTable.clear();

  QueryResult ssCommandsResult = ExtraDatabase.PQuery(
      "SELECT id, text, item, icon, parent_menu, type, action, charges, "
      "duration FROM smartstone_commands");

  if (ssCommandsResult) {
    do {
      SmartStoneCommand command = {};
      command.id = (*ssCommandsResult)[0].GetUInt32();
      command.text = (*ssCommandsResult)[1].GetString();
      command.item = (*ssCommandsResult)[2].GetUInt32();
      command.icon = (*ssCommandsResult)[3].GetUInt32();
      command.parent_menu = (*ssCommandsResult)[4].GetUInt32();
      command.type = (*ssCommandsResult)[5].GetUInt32();
      command.action = (*ssCommandsResult)[6].GetUInt32();
      command.charges = (*ssCommandsResult)[7].GetInt32();
      command.duration = (*ssCommandsResult)[8].GetUInt64();

      ssCommands2.push_back(command);

      count++;

    } while (ssCommandsResult->NextRow());
  }

  sLog->outError("Smartstone: loaded %u commands", count);
}

SmartStoneCommand SmartStone::getCommandById(uint32 id) {
  std::vector<SmartStoneCommand> temp(ssCommands2);
  int n = temp.size();
  for (int i = 0; i < n; i++) {
    if (temp[i].id == id)
      return temp[i];
  }
  return nullCommand;
};

SmartStoneCommand SmartStone::getCommandByItem(uint32 item) {
  std::vector<SmartStoneCommand> temp(ssCommands2);
  int n = temp.size();
  for (int i = 0; i < n; i++) {
    if (temp[i].item == item)
      return temp[i];
  }
  return nullCommand;
};

bool SmartStone::isNullCommand(SmartStoneCommand command) {
  return (command.id == NULL && command.text == "" && command.item == NULL &&
          command.icon == NULL && command.parent_menu == NULL &&
          command.type == NULL && command.action == NULL);
};

SmartStonePlayerCommand SmartStone::toPlayerCommand(SmartStoneCommand command) {
  SmartStonePlayerCommand result;
  result.id = command.id;
  result.charges = command.charges;
  // result.duration = command.duration * 60 + time(NULL);
  result.duration = 0;
  return result;
};

class azth_smartstone_world : public WorldScript {
public:
  azth_smartstone_world() : WorldScript("azth_smartstone_world") {}

  void OnAfterConfigLoad(bool reload) override { sSmartStone->loadCommands(); }
};

class azth_smartstone_player_commands : public PlayerScript {
public:
  azth_smartstone_player_commands()
      : PlayerScript("azth_smartstone_player_commands") {}

  void OnLogin(Player *player) override {
    QueryResult ssCommandsResult = CharacterDatabase.PQuery(
        "SELECT command, dateExpired, charges FROM "
        "character_smartstone_commands WHERE playerGuid = %u ;",
        player->GetGUID());

    if (ssCommandsResult) {
      do {
        uint32 id = (*ssCommandsResult)[0].GetUInt32();
        uint64 date = (*ssCommandsResult)[1].GetUInt64();
        int32 charges = (*ssCommandsResult)[2].GetInt32();
        player->azthPlayer->addSmartStoneCommand(id, false, date, charges);
      } while (ssCommandsResult->NextRow());
    }
  }
};

void SmartStone::SmartStoneSendListInventory(WorldSession *session) {
  int vendorGuid = 1;
  VendorItemData const *items =
      sObjectMgr->GetNpcVendorItemList(SMARTSTONE_VENDOR_ENTRY);
  if (!items) {
    WorldPacket data(SMSG_LIST_INVENTORY, 8 + 1 + 1);
    data << uint64(vendorGuid);
    data << uint8(0); // count == 0, next will be error code
    data << uint8(0); // "Vendor has no inventory"
    session->SendPacket(&data);
    return;
  }

  uint8 itemCount = items->GetItemCount();
  uint8 count = 0;

  WorldPacket data(SMSG_LIST_INVENTORY, 8 + 1 + itemCount * 8 * 4);
  data << uint64(vendorGuid);

  size_t countPos = data.wpos();
  data << uint8(count);

  for (uint8 slot = 0; slot < itemCount; ++slot) {
    if (VendorItem const *item = items->GetItem(slot)) {
      if (ItemTemplate const *itemTemplate =
              sObjectMgr->GetItemTemplate(item->item)) {
        if (!(itemTemplate->AllowableClass &
              session->GetPlayer()->getClassMask()) &&
            itemTemplate->Bonding == BIND_WHEN_PICKED_UP &&
            !session->GetPlayer()->IsGameMaster())
          continue;
        // Only display items in vendor lists for the team the
        // player is on. If GM on, display all items.
        if (!session->GetPlayer()->IsGameMaster() &&
            ((itemTemplate->Flags2 & ITEM_FLAGS_EXTRA_HORDE_ONLY &&
              session->GetPlayer()->GetTeamId() == TEAM_ALLIANCE) ||
             (itemTemplate->Flags2 == ITEM_FLAGS_EXTRA_ALLIANCE_ONLY &&
              session->GetPlayer()->GetTeamId() == TEAM_HORDE)))
          continue;

        uint32 leftInStock = 0xFFFFFFFF;

        std::vector<SmartStonePlayerCommand> playerCommands =
            session->GetPlayer()->azthPlayer->getSmartStoneCommands();
        int n = playerCommands.size();
        SmartStoneCommand command = sSmartStone->getCommandByItem(item->item);

        // I comandi che il player gi� ha li oscuriamo
        for (int i = 0; i < n; i++) {
          // sLog->outError("Smartstone: isnullcommand: %u, command: %u,
          // playercommand: %u", isNullCommand(command), command.id,
          // playerCommands[i]);

          if (!isNullCommand(command) && command.id == playerCommands[i].id)
            leftInStock = 0;
        }

        /* if (!session->GetPlayer()->IsGameMaster() && !leftInStock)
             continue;*/

        /*ConditionList conditions =
        sConditionMgr->GetConditionsForNpcVendorEvent(SMARTSTONE_VENDOR_ENTRY,
        item->item);
        if (!sConditionMgr->IsObjectMeetToConditions(session->GetPlayer(),
        vendor, conditions))
        {
            sLog->outError("SendListInventory: conditions not met for creature
        entry %u item %u", vendor->GetEntry(), item->item);
            continue;
        }*/

        // reputation discount
        int32 price = item->IsGoldRequired(itemTemplate)
                          ? uint32(floor(itemTemplate->BuyPrice))
                          : 0;

        data << uint32(slot + 1); // client expects counting to start at 1
        data << uint32(item->item);
        data << uint32(itemTemplate->DisplayInfoID);
        data << int32(leftInStock);
        data << uint32(price);
        data << uint32(itemTemplate->MaxDurability);
        data << uint32(itemTemplate->BuyCount);
        data << uint32(item->ExtendedCost);

        if (++count >= MAX_VENDOR_ITEMS)
          break;
      }
    }
  }

  if (count == 0) {
    data << uint8(0);
    session->SendPacket(&data);
    return;
  }

  data.put<uint8>(countPos, count);
  session->SendPacket(&data);
}

void AddSC_azth_smart_stone() // Add to scriptloader normally
{
  new azth_smart_stone();
  new azth_smartstone_world();
  new azth_smartstone_player_commands();
}
