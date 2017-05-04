#include "ScriptMgr.h"
#include "Player.h"
#include "Item.h"
#include "ConditionMgr.h"
#include "ItemInBank.h"
#include "ItemToSell.h"

std::vector<ItemToSell> ItemToSellList;
std::vector<ItemInBank> ItemInBankList;

std::map<uint32, std::map<uint32, std::string>> itemTypePositions;

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
        if (!player->azthPlayer->GetTimeWalkingLevel() > 0 && !player->azthPlayer->hasGear())
        {
            player->ADD_GOSSIP_ITEM(0, "Deposita item", GOSSIP_SENDER_MAIN, 501);
        }
        player->ADD_GOSSIP_ITEM_EXTENDED(0, "Inserisci gli item da cercare qui", GOSSIP_SENDER_MAIN, 500, "Inserisci nome item", 0, true);
        player->SEND_GOSSIP_MENU(1, creature->GetGUID());
        return true;
    }


    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
      
        // <---- first step 
        player->PlayerTalkClass->ClearMenus();

        uint32 INVENTORY_ITEMS = 254;
        if (action == 501 && sender == GOSSIP_SENDER_MAIN)
        {
            std::map<uint32, std::string> categoryNames;

            for (uint32 position = 0; position < INVENTORY_ITEMS; position++)
            {
                Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, position);
                uint32 counter = 0;

                if (item != NULL)
                {
                    uint32 inventoryType = item->GetTemplate()->InventoryType;

                    std::vector<std::string> category = getCategoryIconAndNameByItemType(inventoryType); //is an array check iteminbank.h
                    std::string categoryName = category[0];
                    std::string categoryIcon = category[1];


                    if (categoryNames[inventoryType].length() == 0 && counter < 26)
                    {
                        categoryNames[inventoryType] = categoryName;
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/" + categoryIcon + ":30:30:-18:0|t" + categoryName, GOSSIP_SENDER_INFO, inventoryType);
                        counter++;
                    }

                    itemTypePositions[inventoryType][position] = item->GetTemplate()->Name1;
                }
            }
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/PaperDollInfoFrame/UI-GearManager-Undo:30:30:-18:0|tUpdate menu", GOSSIP_SENDER_MAIN, action);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|tBack..", GOSSIP_SENDER_MAIN, 300);
            player->SEND_GOSSIP_MENU(2, creature->GetGUID());
        }
        else if (sender == GOSSIP_SENDER_INFO)
        {
            uint32 inventoryType = action;
            uint32 counter = 0;

            for (std::map<uint32, std::map<uint32, std::string>>::iterator it = itemTypePositions.begin(); it != itemTypePositions.end(); ++it)
            {
                if (it->first == inventoryType)
                {
                    for (std::map<uint32, std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
                    {
                        if (counter < 26)
                        {
                            uint32 itemPosition = it2->first;
                            Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, itemPosition);
                            std::string itemName = it2->second;
                            std::string itemIcon = GetItemIcon(item->GetEntry(), 30, 30, -18, 0);
                            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, itemIcon + itemName, GOSSIP_SENDER_SEC_BANK, itemPosition);
                            counter++;
                        }
                        
                    }
                }
            }
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/PaperDollInfoFrame/UI-GearManager-Undo:30:30:-18:0|tUpdate menu", GOSSIP_SENDER_INFO, action);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|tBack..", GOSSIP_SENDER_MAIN, 301);
            player->SEND_GOSSIP_MENU(10, creature->GetGUID());
        }
        else if (sender == GOSSIP_SENDER_SEC_BANK)
        {
            //player->PlayerTalkClass->SendCloseGossip();

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

            ChatHandler(player->GetSession()).PSendSysMessage("L'item è stato depositato.");//, item->GetTemplate()->Name1);
        }
        else if (action == 300) // go to main menu
        {
            OnGossipHello(player, creature);
        }
        else if (action == 301) // go to categories menu
        {
            OnGossipSelect(player, creature, GOSSIP_SENDER_MAIN, 501);
        }
        
        // -------->

        return true;
    }


    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code)
    {
        
        player->PlayerTalkClass->ClearMenus();
        if (action == 500 && sender == GOSSIP_SENDER_MAIN)
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
