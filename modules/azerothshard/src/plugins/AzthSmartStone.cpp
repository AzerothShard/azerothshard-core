#include "AzthSmartStone.h"

class azth_smart_stone : public ItemScript
{
public:
    azth_smart_stone() : ItemScript("azth_smart_stone") {}

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets)
    {
        player->PlayerTalkClass->ClearMenus();

        std::vector<int> playerCommands = player->azthPlayer->getSmartStoneCommands();
        int n = playerCommands.size();

        for (int i = 0; i < n; i++)
        {
            int command = playerCommands[i];
            std::string text = "Comando senza testo";
            if (sSmartStone->ssCommands.find(command) != sSmartStone->ssCommands.end())
                text = sSmartStone->ssCommands[command];
            
            player->ADD_GOSSIP_ITEM(2, text, GOSSIP_SENDER_MAIN, command);
        }

        player->ADD_GOSSIP_ITEM(0, "|TInterface/ICONS/INV_Misc_Coin_03:30|t Azeroth Store", GOSSIP_SENDER_MAIN, 2000);


        // acquista app

        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, item->GetGUID());

        return true;
    }

    void OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

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
        default:
            sLog->outError("Smartstone: unhandled command! ID: %u, player GUID: %u", action, player->GetGUID());
            break;
        }
        player->CLOSE_GOSSIP_MENU();
    }
};

void SmartStone::loadCommands()
{
    // initialize count and array
    uint32 count = 0;
    //sHearthstoneMode->hsAchievementTable.clear();

    QueryResult ssCommandsResult = ExtraDatabase.PQuery("SELECT id, text, item FROM smartstone_commands");

    if (ssCommandsResult)
    {
        do
        {
            SmartStoneCommands command = {};
            command.id = (*ssCommandsResult)[0].GetUInt32();
            command.text = (*ssCommandsResult)[1].GetString();
            command.item = (*ssCommandsResult)[2].GetUInt32();

            // sSmartStone->ssCommands.push_back(command);
            ssCommands[command.id] = command.text;
            ssCommandsItemRelation[command.item] = command.id;

            count++;

        } while (ssCommandsResult->NextRow());
    }

    sLog->outError("Smartstone: loaded %u commands", count);
}

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
                player->azthPlayer->addSmartStoneCommand((*ssCommandsResult)[0].GetUInt32(), false);
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

                // I comandi che il player già ha li skippiamo
                uint32 leftInStock = 1;

                std::vector<int> playerCommands = session->GetPlayer()->azthPlayer->getSmartStoneCommands();
                int n = playerCommands.size();
                

                for (int i = 0; i < n; i++)
                {
                    if (sSmartStone->ssCommandsItemRelation.find(item->item) != sSmartStone->ssCommandsItemRelation.end())
                        if (sSmartStone->ssCommandsItemRelation[item->item] == playerCommands[i])
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
