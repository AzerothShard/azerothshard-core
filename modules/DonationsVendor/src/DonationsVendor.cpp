#include "ScriptMgr.h"
#include "Player.h"
#include "Item.h"
#include "ConditionMgr.h"
#include "ItemInBank.h"
#include "ItemToSell.h"

std::vector<ItemToSell> ItemToSellList;
std::vector<ItemInBank> ItemInBankList;

class loadItemVendor : public WorldScript
{
public:
    loadItemVendor() : WorldScript("loadItemVendor") {}
    void OnStartup()
    {
        QueryResult partialItemListQuery = WorldDatabase.PQuery("SELECT a.entry,a.name,b.ExtendedCost FROM item_template AS a LEFT JOIN npc_vendor AS b ON b.entry = %d AND a.entry=b.item;", NPC_DONATIONS_VENDOR_ID ); //retrieve all items from db
        Field* partialItemListField = partialItemListQuery->Fetch();
        bool canBeBought;

        do {
            uint32 id = partialItemListField[0].GetUInt32();
            string name = partialItemListField[1].GetString();
            uint32 extCost = partialItemListField[2].GetUInt32();
            QueryResult itemToSellQuery = WorldDatabase.PQuery("SELECT ExtendedCost FROM npc_vendor WHERE entry=%d AND item =%d;", NPC_DONATIONS_VENDOR_ID, id); //if result is empty, item cannot be purchased
            
            ItemTemplate const* _proto = sObjectMgr->GetItemTemplate(id);
            if (!_proto)
                return;
    
            canBeBought = extCost > 0 ? true : false;

            ItemToSellList.push_back(ItemToSell(id, name, extCost, canBeBought)); //push data into ItemToSellList list

        } while (partialItemListQuery->NextRow());
        
        //----------------------------------------------------------------------------

        QueryResult itemInBankQuery = CharacterDatabase.PQuery("SELECT * FROM azth_items_bank"); //retrieve all items from db

        if (itemInBankQuery)
        {
            Field* itemInBankField = itemInBankQuery->Fetch();

            do {
                uint32 characterGUID = itemInBankField[0].GetUInt32();
                uint32 itemGUID = itemInBankField[1].GetUInt32();
                uint32 itemEntry = itemInBankField[2].GetUInt32();

                //aggiungere check da altra tabella per controllare se lo possedeva

                ItemInBankList.push_back(ItemInBank(characterGUID, itemGUID, itemEntry));

            } while (partialItemListQuery->NextRow());

        }
        
        sItemInBank->SetBankItemsList(ItemInBankList);
    }
};


class DonatorVendor : public CreatureScript
{
public:
    DonatorVendor() : CreatureScript("DonatorVendor") {}

    bool OnGossipHello(Player* player, Creature* creature)
    {
        //                              icon            text                           sender      action   popup message    money  code
        if (!player->azthPlayer->GetTimeWalkingLevel() > 0 || !player->azthPlayer->hasGear())
        {
            player->ADD_GOSSIP_ITEM(0, "Deposita item", GOSSIP_SENDER_MAIN, 300);
        }
        player->ADD_GOSSIP_ITEM_EXTENDED(0, "Inserisci gli item da cercare qui", GOSSIP_SENDER_MAIN, 1, "Inserisci nome item", 0, true);
        player->SEND_GOSSIP_MENU(1, creature->GetGUID());
        return true;
    }


    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        // <---- first step 
        player->PlayerTalkClass->ClearMenus();

        uint32 INVENTORY_ITEMS = 254;
        if (action == 300)
        {
            for (uint32 position = 0; position < INVENTORY_ITEMS; position++)
            {
                Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, position);
                if (item != NULL)
                {
                    string itemName = item->GetTemplate()->Name1;
                    player->ADD_GOSSIP_ITEM(0, /*"Deposita " + */itemName, GOSSIP_SENDER_MAIN, position);
                }
            }
            player->SEND_GOSSIP_MENU(2, creature->GetGUID());
        }
        // --------->
        // <---- second step
        else
        {
            uint32 slot = action;

            Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);

            if (sItemToSell->OwnItem(player->GetGUID(), item->GetEntry()))
            {
                return true;
                //player already deposited this item
            }

            //instant remove
            player->MoveItemFromInventory(INVENTORY_SLOT_BAG_0, slot, true);
           
            //delete from inventory
            SQLTransaction trans = CharacterDatabase.BeginTransaction();
            item->DeleteFromInventoryDB(trans); 
            item->SaveToDB(trans);
            CharacterDatabase.CommitTransaction(trans);

            //add item to list of owned items
            vector<ItemInBank> bankItemList = sItemInBank->GetBankItemsList();
            bankItemList.push_back(ItemInBank(player->GetGUID(), item->GetGUID(), item->GetEntry()));
            sItemInBank->SetBankItemsList(bankItemList); //update list

            CharacterDatabase.PQuery("INSERT INTO azth_items_bank (`guid`, `item`, `itemEntry`) VALUES (%d, %d, %d);", player->GetGUID(), item->GetGUID(), item->GetEntry());

            ChatHandler(player->GetSession()).PSendSysMessage("L'item e' stato depositato.");//, item->GetTemplate()->Name1);
        }

        // -------->

        return true;
    }


    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code)
    {
        
        player->PlayerTalkClass->ClearMenus();
        if (action == 1)
        {
            std::vector<ItemToSell> allItems;
            std::vector<ItemToSell> NotBuyableItems;
            for (uint32 i = 0; i < ItemToSellList.size(); i++)
            {
                string completeString = ItemToSellList[i].GetName();
                string partialString = code;
                string partialStringFirstLetterUpper = sItemToSell->CapitalizeFirstLetterEveryWord(code);
                if (completeString.find(partialString) != string::npos || completeString.find(partialStringFirstLetterUpper) != string::npos)
                {
                    if (ItemToSellList[i].GetCanBeBought())
                    {
                        allItems.push_back(ItemToSell(ItemToSellList[i].GetId(), ItemToSellList[i].GetName(), ItemToSellList[i].GetExtCost(), ItemToSellList[i].GetCanBeBought()));
                    }
                    else
                    {
                        NotBuyableItems.push_back(ItemToSell(ItemToSellList[i].GetId(), ItemToSellList[i].GetName(), ItemToSellList[i].GetExtCost(), ItemToSellList[i].GetCanBeBought()));
                    }
                }
            }

            allItems.insert(allItems.end(), NotBuyableItems.begin(), NotBuyableItems.end());

            if (allItems.size() == 0)
            {
                player->SEND_GOSSIP_MENU(NO_ITEM_FOUND, creature->GetGUID());
            }
            else
            {
                sItemToSell->SendListInventoryDonorVendor(player->GetSession(), creature->GetGUID(), allItems, player);
            }
        }
        return true;
    }
};

class DonatorVendorPlayer : public PlayerScript
{
public:
    DonatorVendorPlayer() : PlayerScript("DonatorVendorPlayer") {}

    void OnBeforeBuyItemFromVendor(Player* player, uint64 vendorguid, uint32 vendorslot, uint32 &itemEntry /*itemEntry*/, uint8 count, uint8 bag, uint8 slot)
    {
        Creature *vendor = player->GetMap()->GetCreature(vendorguid);

        if (vendor->GetScriptName() == "DonatorVendor")
        {
            uint32 guid = sItemToSell->OwnItem(player->GetGUID(), itemEntry);

            if (guid != 0)
            {
                ItemTemplate const* _proto = sObjectMgr->GetItemTemplate(itemEntry);
                if (!_proto)
                    return;

                Item* pItem = NewItemOrBag(_proto);

                QueryResult selectItemQuery = CharacterDatabase.PQuery("SELECT creatorGuid, giftCreatorGuid, count, duration, charges, flags, enchantments, randomPropertyId, durability, playedTime, text, itemEntry FROM item_instance WHERE guid = %d", guid);
                Field* selectItemField = selectItemQuery->Fetch();

                pItem->LoadFromDB(guid, player->GetGUID(), selectItemField, itemEntry);

                ItemPosCountVec dest;

                SQLTransaction trans = CharacterDatabase.BeginTransaction();

                if (player->CanStoreItem(NULL_BAG, NULL_SLOT, dest, pItem, false) == EQUIP_ERR_OK)
                    player->MoveItemToInventory(dest, pItem, true, false);
                else
                    sLog->outError("player can't take item back: %u", pItem->GetGUIDLow());

                player->SaveInventoryAndGoldToDB(trans);
                CharacterDatabase.CommitTransaction(trans);

                CharacterDatabase.PQuery("DELETE FROM `azth_items_bank` WHERE `guid`=%d AND `itemEntry`=%d;", player->GetGUID(), itemEntry);

                vector<ItemInBank> itemInBankList = sItemInBank->GetBankItemsList();

                for (std::vector<ItemInBank>::iterator it = itemInBankList.begin(); it != itemInBankList.end(); ++it)
                {
                    if (it->GetCharacterGUID() == player->GetGUID() && it->GetItemEntry() == itemEntry)
                    {
                        itemInBankList.erase(it);
                        break;
                    }
                }

                sItemInBank->SetBankItemsList(itemInBankList);

            }
        }
    }
};


void AddSC_DonationsVendor()
{
    new loadItemVendor();
    new DonatorVendor();
    new DonatorVendorPlayer();
}
