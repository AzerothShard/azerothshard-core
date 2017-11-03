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
#include "MapManager.h"
#include "Map.h"

#include "Apps/Apps.h"

enum SmartStoneCommands {
    SMRTST_BLACK_MARKET=1,
    SMRTST_CHANGE_FACTION=2,
    SMRTST_RENAME=3,
    SMRTST_CHAR_MENU=4,
    SMRTST_CHANGE_RACE=5,
    SMRTST_JUKEBOX=6,
    SMRTST_HERBALISM_BONUS=7,
    SMRTST_MINING_BONUS=8,
    SMRTST_BONUS_MENU=9,
    SMRTST_MAX_SKILL=10,
    SMRTST_XP_CHANGE=11,
    SMRTST_RESET_AURAS=12,
    SMRTST_TELEPORT_DALARAN=13,
    SMRTST_SHOP_MENU=2000, //unused
    SMRTST_BACK_MENU=2001,
};

std::string SmartStoneCommand::getText(Player *pl) {
        AzthCustomLangs loc = AZTH_LOC_IT;

        if (pl)
            loc = pl->azthPlayer->getCustLang();
        
        switch(loc) {
            case AZTH_LOC_IT:
                return text_it;
            break;
            case AZTH_LOC_EN:
                return text_def;
            break;
        }
        
        return text_def;
}

class azth_smart_stone : public ItemScript {
public:
    
    uint32 parent = 1;
    SmartStoneApps *apps;

    azth_smart_stone() : ItemScript("azth_smart_stone") {
        apps = new SmartStoneApps();
    }
    
    
    void OnGossipSelect(Player *player, Item *item, uint32  /*sender*/,
            uint32 action) override {
        player->PlayerTalkClass->ClearMenus();

        // back to main menu command
        if (action == 2001) {
            parent = 1;
            player->CLOSE_GOSSIP_MENU();
            OnUse(player, item, SpellCastTargets());
        }

        SmartStoneCommand selectedCommand = sSmartStone->getCommandById(action);

        // scripted action
        if (selectedCommand.type == DO_SCRIPTED_ACTION ||
                action == 2000) // azeroth store
        {
            switch (action) {
                case SMRTST_SHOP_MENU: // store
                    //sSmartStone->SmartStoneSendListInventory(player->GetSession());
                break;
                case SMRTST_BLACK_MARKET: // black market teleport
                    apps->blackMarketTeleport(player);
                break;

                case SMRTST_CHANGE_FACTION: // change faction
                    apps->changeFaction(player);
                break;

                case SMRTST_RENAME: // rename
                    apps->rename(player);
                break;

                case SMRTST_CHANGE_RACE: // change race
                    apps->changeRace(player);
                break;

                case SMRTST_JUKEBOX: // jukebox
                    apps->jukebox(player);
                break;

                case SMRTST_MAX_SKILL: // maxskill
                    apps->maxSkill(player);
                break;
                
                case SMRTST_RESET_AURAS:
                    apps->resetAuras(player);
                break;
                
                case SMRTST_TELEPORT_DALARAN:
                    apps->teleportDalaran(player);
                break;
                case 99999:
                    break;
                default:
                    sLog->outError("Smartstone: unhandled command! ID: %u, player GUID: %lu",
                            action, player->GetGUID());
                    break;
            }
            if (selectedCommand.charges > 0) {
                player->azthPlayer->decreaseSmartStoneCommandCharges(
                        selectedCommand.id);
            }
            player->CLOSE_GOSSIP_MENU();
            // return;
        }

        // open child
        if (selectedCommand.type == OPEN_CHILD) {
            parent = selectedCommand.action;
            player->CLOSE_GOSSIP_MENU();
            OnUse(player, item, SpellCastTargets());
        }
    }

    void OnGossipSelectCode(Player* player, Item*  /*item*/, uint32  /*sender*/, uint32 action, const char* code) override {
        player->PlayerTalkClass->ClearMenus();

        SmartStoneCommand selectedCommand = sSmartStone->getCommandById(action);

        // scripted action
        if (selectedCommand.type == DO_SCRIPTED_ACTION_WITH_CODE || action == 2000) // azeroth store
        {
            switch (action) {
                case SMRTST_XP_CHANGE: //change exp
                    apps->changeExp(player, code);
                break;

                case 99999:
                    break;
                default:
                    sLog->outError("Smartstone: unhandled command with code! ID: %u, player GUID: %lu", action, player->GetGUID());
                    break;
            }
            if (selectedCommand.charges > 0) {
                player->azthPlayer->decreaseSmartStoneCommandCharges(selectedCommand.id);
            }
            player->CLOSE_GOSSIP_MENU();
        }
    }

    bool OnUse(Player *player, Item *item, SpellCastTargets const & /*targets*/) override {
        player->PlayerTalkClass->ClearMenus();

        if (parent == 1) // not-to-buy commands for the main menu
        {
            // black market teleport id 1
            SmartStoneCommand teleport = sSmartStone->getCommandById(1);
            if (!player->azthPlayer->isInBlackMarket())
                player->ADD_GOSSIP_ITEM(teleport.icon, teleport.getText(player), GOSSIP_SENDER_MAIN, teleport.id);
            else
                player->ADD_GOSSIP_ITEM(teleport.icon, "Riportami indietro", GOSSIP_SENDER_MAIN, teleport.id);

            // menu character (rename, change faction, etc) id 4
            SmartStoneCommand characterMenu = sSmartStone->getCommandById(4);
            player->ADD_GOSSIP_ITEM(characterMenu.icon, characterMenu.getText(player),
                    GOSSIP_SENDER_MAIN, characterMenu.id);

            // menu passive bonus id 9
            SmartStoneCommand passiveMenu = sSmartStone->getCommandById(9);
            player->ADD_GOSSIP_ITEM(passiveMenu.icon, passiveMenu.getText(player),
                    GOSSIP_SENDER_MAIN, passiveMenu.id);
        }

        if (parent == 2) // not-to-buy commands for the characters menu
        {
            // max skill command
            SmartStoneCommand maxSkill = sSmartStone->getCommandById(10);
            player->ADD_GOSSIP_ITEM(maxSkill.icon, maxSkill.getText(player), GOSSIP_SENDER_MAIN, maxSkill.id);

            // azth xp command
            SmartStoneCommand azthXp = sSmartStone->getCommandById(11);
            player->ADD_GOSSIP_ITEM_EXTENDED(azthXp.icon, azthXp.getText(player), GOSSIP_SENDER_MAIN, azthXp.id, "Scrivi il valore desiderato.", 0, true);
            
            // reset auras
            SmartStoneCommand resetAuras = sSmartStone->getCommandById(12);
            player->ADD_GOSSIP_ITEM(resetAuras.icon, resetAuras.getText(player), GOSSIP_SENDER_MAIN, resetAuras.id);
            
            if (player->azthPlayer->isPvP()) {
                // dalaran teleport
                SmartStoneCommand dalaranTeleport = sSmartStone->getCommandById(13);
                player->ADD_GOSSIP_ITEM(dalaranTeleport.icon, dalaranTeleport.getText(player), GOSSIP_SENDER_MAIN, dalaranTeleport.id);
            }
        }

        std::vector<SmartStonePlayerCommand> & playerCommands =
                player->azthPlayer->getSmartStoneCommands();
        int n = playerCommands.size();

        for (int i = 0; i < n; i++) {
            SmartStoneCommand command =
                    sSmartStone->getCommandById(playerCommands[i].id);

            // if expired or no charges
            if ((playerCommands[i].duration <= uint32(time(NULL)) &&
                    playerCommands[i].duration != 0) ||
                    playerCommands[i].charges == 0) {
                player->azthPlayer->removeSmartStoneCommand(playerCommands[i], true);
                continue;
            }

            std::string text = command.getText(player);

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

            if (command.id != 0 && command.parent_menu == parent) {
                if (command.type != DO_SCRIPTED_ACTION_WITH_CODE) {
                    player->ADD_GOSSIP_ITEM(command.icon, text, GOSSIP_SENDER_MAIN, command.id);
                } else {
                    player->ADD_GOSSIP_ITEM_EXTENDED(command.icon, text, GOSSIP_SENDER_MAIN, command.id, "Scrivi il valore desiderato.", 0, true);
                }
            }
        }

        // acquista app

        /*if (parent == 1)
          player->ADD_GOSSIP_ITEM(
              0, "|TInterface/ICONS/INV_Misc_Coin_03:30|t Azeroth Store",
              GOSSIP_SENDER_MAIN, 2000);*/

        if (parent != 1) {
            // back to main menu command
            player->ADD_GOSSIP_ITEM(0, "Indietro", GOSSIP_SENDER_MAIN, 2001);
        }

        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, item->GetGUID());

        parent = 1;
        return true;
    }
};

void SmartStone::loadCommands() {
    // initialize count and array
    uint32 count = 0;
    // sHearthstoneMode->hsAchievementTable.clear();

    QueryResult ssCommandsResult = ExtraDatabase.PQuery(
            "SELECT id, text_def, text_it, item, icon, parent_menu, type, action, charges, "
            "duration FROM smartstone_commands");

    if (ssCommandsResult) {
        do {
            SmartStoneCommand command = {};
            command.id = (*ssCommandsResult)[0].GetUInt32();
            command.text_def = (*ssCommandsResult)[1].GetString();
            command.text_it = (*ssCommandsResult)[2].GetString();
            command.item = (*ssCommandsResult)[3].GetUInt32();
            command.icon = (*ssCommandsResult)[4].GetUInt32();
            command.parent_menu = (*ssCommandsResult)[5].GetUInt32();
            command.type = (*ssCommandsResult)[6].GetUInt32();
            command.action = (*ssCommandsResult)[7].GetUInt32();
            command.charges = (*ssCommandsResult)[8].GetInt32();
            command.duration = (*ssCommandsResult)[9].GetUInt64();

            ssCommands2.push_back(command);

            count++;

        } while (ssCommandsResult->NextRow());
    }

    sLog->outString("Smartstone: loaded %u commands", count);
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
    return (command.id == 0 && command.text_def == "" && command.item == 0 &&
            command.icon == 0 && command.parent_menu == 0 &&
            command.type == 0 && command.action == 0);
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

    azth_smartstone_world() : WorldScript("azth_smartstone_world") {
    }

    void OnAfterConfigLoad(bool  /*reload*/) override {
        sSmartStone->loadCommands();
    }
};

class azth_smartstone_player_commands : public PlayerScript {
public:

    azth_smartstone_player_commands()
    : PlayerScript("azth_smartstone_player_commands") {
    }

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

        std::map<uint32,WorldLocation> pos = player->azthPlayer->getLastPositionInfoFromDB();
        
        uint32 dimension=player->azthPlayer->getCurrentDimensionByAura();
        if (pos.find(dimension) != pos.end())
            player->azthPlayer->setLastPositionInfo(dimension, pos[dimension]);
        else
            player->azthPlayer->setLastPositionInfo(dimension, AzthSharedDef::blackMarket);
    }

    void OnLogout(Player* player) override {
        player->azthPlayer->saveLastPositionInfoToDB(player);
    }

    void OnBeforeBuyItemFromVendor(Player* player, uint64 vendorguid, uint32 vendorslot, uint32 &item, uint8 count, uint8  /*bag*/, uint8 /*slot*/) override {
        if (!sSmartStone->isNullCommand(sSmartStone->getCommandByItem(item))) {
            player->azthPlayer->BuySmartStoneCommand(vendorguid, vendorslot, item, count, NULL_BAG, NULL_SLOT);
            item = 0;
        }
    }
};

class smartstone_vendor : public CreatureScript {
public:

    smartstone_vendor() : CreatureScript("smartstone_vendor") {
    }

    bool OnGossipHello(Player* player, Creature* creature) override {
        player->PlayerTalkClass->ClearMenus();

        player->ADD_GOSSIP_ITEM(0, "Salve, vorrei comprare nuove app!", GOSSIP_SENDER_MAIN, 1);
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override {
        player->PlayerTalkClass->ClearMenus();

        if (action == 1)
            sSmartStone->SmartStoneSendListInventory(player->GetSession(), creature->GetGUID());

        return true;
    }
};

void SmartStone::SmartStoneSendListInventory(WorldSession *session, uint64 vendorGuid) {
    VendorItemData const *items = sObjectMgr->GetNpcVendorItemList(SMARTSTONE_VENDOR_ENTRY);



    if (!items) {
        WorldPacket data(SMSG_LIST_INVENTORY, 8 + 1 + 1);
        data << vendorGuid;
        data << uint8(0); // count == 0, next will be error code
        data << uint8(0); // "Vendor has no inventory"
        session->SendPacket(&data);
        return;
    }

    uint8 itemCount = items->GetItemCount();
    uint8 count = 0;

    WorldPacket data(SMSG_LIST_INVENTORY, 8 + 1 + itemCount * 8 * 4);
    data << vendorGuid;

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

                std::vector<SmartStonePlayerCommand> & playerCommands =
                        session->GetPlayer()->azthPlayer->getSmartStoneCommands();
                int n = playerCommands.size();
                SmartStoneCommand command = sSmartStone->getCommandByItem(item->item);

                // we hide commands that the player already has
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
    new smartstone_vendor();
}