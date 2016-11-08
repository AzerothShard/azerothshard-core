#include "AzthSmartStone.h"
#include "Player.h"
#include "WorldSession.h"
#include "WorldPacket.h"
#include "Chat.h"
#include "Spell.h"
#include "Define.h"
#include "GossipDef.h"
#include "Item.h"
#include "Common.h"
#include "Opcodes.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"

uint32 parent = 1;

class azth_smart_stone : public ItemScript
{
public:
    azth_smart_stone() : ItemScript("azth_smart_stone") {}

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets)
    {
        player->PlayerTalkClass->ClearMenus();

        player->ADD_GOSSIP_ITEM(0, "Benvenuto nella tua SmartStone!", GOSSIP_SENDER_MAIN, 99999);

        std::vector<SmartStoneCommand> playerCommands = player->azthPlayer->getSmartStoneCommands();
        int n = playerCommands.size();

        for (int i = 0; i < n; i++)
        {
            SmartStoneCommand command = playerCommands[i];
            if (!command.id == 0 && command.parent_menu == parent)
                player->ADD_GOSSIP_ITEM(command.icon, command.text, GOSSIP_SENDER_MAIN, command.id);
        }

        // acquista app

        if (parent == 1)
            player->ADD_GOSSIP_ITEM(0, "|TInterface/ICONS/INV_Misc_Coin_03:30|t Azeroth Store", GOSSIP_SENDER_MAIN, 2000);

        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, item->GetGUID());

        parent = 1;
        return true;
    }

    void OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        SmartStoneCommand selectedCommand = sSmartStone->getCommandById(action);

        // scripted action
        if (selectedCommand.type == DO_SCRIPTED_ACTION || action == 2000) // azeroth store
        {
            switch (action)
            {
            case 2000: // store
                sSmartStone->SmartStoneSendListInventory(player->GetSession(), 100000);
                break;
            case 1:
                if (!player->IsInCombat())
                    player->TeleportTo(1, 4818.27f, -1971.3f, 1069.75f, 0.174f, 0);
                break;
            case 2:
                player->DeMorph();
                break;
            case 3:
                player->SetDisplayId(1060);
                break;
            case 99999:
                break;
            default:
                sLog->outError("Smartstone: unhandled command! ID: %u, player GUID: %u", action, player->GetGUID());
                break;
            }
            player->CLOSE_GOSSIP_MENU();
           // return;
        }

        // open child
        if (selectedCommand.type == OPEN_CHILD)
        {
            parent = selectedCommand.action;
            player->CLOSE_GOSSIP_MENU();
            OnUse(player, item, SpellCastTargets());
            /*
            player->PlayerTalkClass->ClearMenus();

            std::vector<SmartStoneCommand> playerCommands = player->azthPlayer->getSmartStoneCommands();
            int n = playerCommands.size();
           

            for (int i = 0; i < n; i++)
            {
                SmartStoneCommand command = playerCommands[i];
                if (!command.id == 0 && command.parent_menu == selectedCommand.action)
                {
                    player->ADD_GOSSIP_ITEM(command.icon, command.text, GOSSIP_SENDER_MAIN, command.id);
                }
            }

            if (selectedCommand.action == 1)
                player->ADD_GOSSIP_ITEM(0, "|TInterface/ICONS/INV_Misc_Coin_03:30|t Azeroth Store", GOSSIP_SENDER_MAIN, 2000);

            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
            */
            
           // return;
        }
    }
};

void SmartStone::loadCommands()
{
    // initialize count and array
    uint32 count = 0;
    //sHearthstoneMode->hsAchievementTable.clear();

    QueryResult ssCommandsResult = ExtraDatabase.PQuery("SELECT id, text, item, icon, parent_menu, type, action FROM smartstone_commands");

    if (ssCommandsResult)
    {
        do
        {
            SmartStoneCommand command = {};
            command.id = (*ssCommandsResult)[0].GetUInt32();
            command.text = (*ssCommandsResult)[1].GetString();
            command.item = (*ssCommandsResult)[2].GetUInt32();
            command.icon = (*ssCommandsResult)[3].GetUInt32();
            command.parent_menu = (*ssCommandsResult)[4].GetUInt32();
            command.type = (*ssCommandsResult)[5].GetUInt32();
            command.action = (*ssCommandsResult)[6].GetUInt32();

            ssCommands2.push_back(command);

            count++;

        } while (ssCommandsResult->NextRow());
    }

    sLog->outError("Smartstone: loaded %u commands", count);
}

SmartStoneCommand SmartStone::getCommandById(uint32 id)
{
    int n = ssCommands2.size();
    for (int i = 0; i < n; i++)
    {
        if (ssCommands2[i].id == id)
            return ssCommands2[i];
    }
    return nullCommand;
};

SmartStoneCommand SmartStone::getCommandByItem(uint32 item)
{
    int n = ssCommands2.size();
    for (int i = 0; i < n; i++)
    {
        if (ssCommands2[i].item == item)
            return ssCommands2[i];
    }
    return nullCommand;
};

bool SmartStone::isNullCommand(SmartStoneCommand command)
{
    return (command.id == NULL && command.text == "" && command.item == NULL && command.icon == NULL 
        && command.parent_menu == NULL && command.type == NULL && command.action == NULL);
};

class azth_smartstone_world : public WorldScript
{
public:
    azth_smartstone_world() : WorldScript("azth_smartstone_world") { }

    void OnAfterConfigLoad(bool reload) override
    {
        sSmartStone->loadCommands();
    }
};

class azth_smartstone_player_commands : public PlayerScript
{
public:
    azth_smartstone_player_commands() : PlayerScript("azth_smartstone_player_commands") { }

    void OnLogin(Player* player) override
    {
        QueryResult ssCommandsResult = 
            CharacterDatabase.PQuery("SELECT command FROM character_smartstone_commands WHERE playerGuid = %u ;", player->GetGUID());

        if (ssCommandsResult)
        {
            do
            {
                player->azthPlayer->addSmartStoneCommand((sSmartStone->getCommandById((*ssCommandsResult)[0].GetUInt32())), false);
            } while (ssCommandsResult->NextRow());
        }
    }
};

void SmartStone::SmartStoneSendListInventory(WorldSession * session, uint32 extendedCostStartValue)
{
    int vendorGuid = 1;
    VendorItemData const* items = sObjectMgr->GetNpcVendorItemList(SMARTSTONE_VENDOR_ENTRY);
    if (!items)
    {
        WorldPacket data(SMSG_LIST_INVENTORY, 8 + 1 + 1);
        data << uint64(vendorGuid);
        data << uint8(0);                                   // count == 0, next will be error code
        data << uint8(0);                                   // "Vendor has no inventory"
        session->SendPacket(&data);
        return;
    }

    uint8 itemCount = items->GetItemCount();
    uint8 count = 0;

    WorldPacket data(SMSG_LIST_INVENTORY, 8 + 1 + itemCount * 8 * 4);
    data << uint64(vendorGuid);

    size_t countPos = data.wpos();
    data << uint8(count);


    for (uint8 slot = 0; slot < itemCount; ++slot)
    {
        if (VendorItem const* item = items->GetItem(slot))
        {
            if (ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(item->item))
            {
                if (!(itemTemplate->AllowableClass & session->GetPlayer()->getClassMask()) && itemTemplate->Bonding == BIND_WHEN_PICKED_UP && !session->GetPlayer()->IsGameMaster())
                    continue;
                // Only display items in vendor lists for the team the
                // player is on. If GM on, display all items.
                if (!session->GetPlayer()->IsGameMaster() && ((itemTemplate->Flags2 & ITEM_FLAGS_EXTRA_HORDE_ONLY && session->GetPlayer()->GetTeamId() == TEAM_ALLIANCE) || (itemTemplate->Flags2 == ITEM_FLAGS_EXTRA_ALLIANCE_ONLY && session->GetPlayer()->GetTeamId() == TEAM_HORDE)))
                    continue;

                
                uint32 leftInStock = 0xFFFFFFFF;

                std::vector<SmartStoneCommand> playerCommands = session->GetPlayer()->azthPlayer->getSmartStoneCommands();
                int n = playerCommands.size();
                SmartStoneCommand command = sSmartStone->getCommandByItem(item->item);

                // I comandi che il player già ha li oscuriamo
                for (int i = 0; i < n; i++)
                {
                    //sLog->outError("Smartstone: isnullcommand: %u, command: %u, playercommand: %u", isNullCommand(command), command.id, playerCommands[i]);
                    

                    if (!isNullCommand(command) && command.id == playerCommands[i].id)
                        leftInStock = 0;
                }
                
               /* if (!session->GetPlayer()->IsGameMaster() && !leftInStock)
                    continue;*/

                /*ConditionList conditions = sConditionMgr->GetConditionsForNpcVendorEvent(SMARTSTONE_VENDOR_ENTRY, item->item);
                if (!sConditionMgr->IsObjectMeetToConditions(session->GetPlayer(), vendor, conditions))
                {
                    sLog->outError("SendListInventory: conditions not met for creature entry %u item %u", vendor->GetEntry(), item->item);
                    continue;
                }*/

                // reputation discount
                uint32 ExtendedToGold = item->ExtendedCost > extendedCostStartValue ? (item->ExtendedCost - extendedCostStartValue) * 10000 : 0;
                int32 price = item->IsGoldRequired(itemTemplate) ? uint32(floor(itemTemplate->BuyPrice)) : ExtendedToGold;

                data << uint32(slot + 1);       // client expects counting to start at 1
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

    if (count == 0)
    {
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
