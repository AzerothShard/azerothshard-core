#include "ScriptMgr.h"
#include "Player.h"
#include "Item.h"
#include "ConditionMgr.h"
#include "ItemToSell.h"
#include "ScriptedGossip.h"
#include "GossipDef.h"
#include "AzthLanguage.h"
#include "AzthUtils.h"

#define AZTH_ITEMBANK_START_ACTION 100000
// same as MAX_VENDOR_ITEMS
#define AZTH_ITEMBANK_RANGE 150

std::vector<ItemToSell> ItemToSellList;

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
    }
};


class DonatorVendor : public CreatureScript
{
    std::map<uint32, std::map<uint32, std::string>> itemTypePositions;

public:
    DonatorVendor() : CreatureScript("DonatorVendor") {}

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        //                              icon            text                           sender      action   popup message    money  code
        if (!(player->azthPlayer->GetTimeWalkingLevel() > 0) && !player->azthPlayer->hasGear())
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Deposita item", GOSSIP_SENDER_MAIN, 501);
        }
        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "Inserisci gli items da cercare qui", GOSSIP_SENDER_MAIN, 500, "Inserisci nome item", 0, true);
        
        // from 1 to 150
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sAzthLang->getf(AZTH_LANG_SHOW_BANK_ITEMS,player,1,150), GOSSIP_SENDER_MAIN, AZTH_ITEMBANK_START_ACTION);
        // from 150 to 300
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sAzthLang->getf(AZTH_LANG_SHOW_BANK_ITEMS,player,150,300), GOSSIP_SENDER_MAIN, AZTH_ITEMBANK_START_ACTION + (AZTH_ITEMBANK_RANGE*1));
        
        player->SEND_GOSSIP_MENU(1, creature->GetGUID());
        return true;
    }


    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
      
        // <---- first step 
        player->PlayerTalkClass->ClearMenus();

        //[0,18] [23,38]  [118,135]

        /*uint32 EQUIPPED_ITEMS_SLOT_END = 18; //0-18
        uint32 BACKPACK_SLOT_START = 23;
        uint32 BACKPACK_SLOT_END = 38;
        uint32 CURRENCIES_SLOT_START = 118;
        uint32 CURRENCIES_SLOT_END = 135;*/
        uint32 SLOT_END = 38;

        if (action == 501 && sender == GOSSIP_SENDER_MAIN)
        {
            std::map<uint32, std::string> categoryNames;

            for (uint32 position = 23; position < SLOT_END; position++)
            {
				Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, position);
				uint32 counter = 0;

				if (item != NULL)
				{
					uint32 inventoryType = item->GetTemplate()->InventoryType;

					std::vector<std::string> category = getCategoryIconAndNameByItemType(inventoryType);
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
                            if (item != NULL)
                            {
                                std::string itemName = it2->second;
                                std::string itemIcon = GetItemIcon(item->GetEntry(), 30, 30, -18, 0);
                                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, itemIcon + itemName, GOSSIP_SENDER_SEC_BANK, itemPosition);
                                counter++;
                            }
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

            if (item->GetCount() > 1)
            {
                ChatHandler(player->GetSession()).PSendSysMessage("Non possono essere depositati item raggruppati in un unico slot (stacked).");
                return true;
            }

            uint32 inventoryType = item->GetTemplate()->InventoryType;

            if (sItemToSell->OwnItem(player, item->GetEntry()))
            {
                return true;
                //player already deposited this item
            }

            //add item to list of owned items
            player->azthPlayer->AddBankItem(item->GetEntry(), item->GetGUID());

            SQLTransaction trans = CharacterDatabase.BeginTransaction();
            item->SaveToDB(trans); // if this item has not been saved yet in character inventory

            //instant remove
            player->MoveItemFromInventory(INVENTORY_SLOT_BAG_0, slot, true);

            //delete from inventory
            item->DeleteFromInventoryDB(trans);
            item->SaveToDB(trans);
            player->SaveInventoryAndGoldToDB(trans);
            CharacterDatabase.CommitTransaction(trans);

            CharacterDatabase.PQuery("INSERT INTO azth_items_bank (`guid`, `item`, `itemEntry`) VALUES (%d, %d, %d);", player->GetGUID(), item->GetGUID(), item->GetEntry());

            ChatHandler(player->GetSession()).PSendSysMessage("Item depositato.");//, item->GetTemplate()->Name1);

            OnGossipSelect(player, creature, GOSSIP_SENDER_INFO, inventoryType);
            //send menu again
        }
        else if (action == 300) // go to main menu
        {
            OnGossipHello(player, creature);
        }
        else if (action == 301) // go to categories menu
        {
            OnGossipSelect(player, creature, GOSSIP_SENDER_MAIN, 501);
        } else if (action >= 100000) {
            return _showBank(creature, player, uint32(action - AZTH_ITEMBANK_START_ACTION));
        }
        
        // -------->

        return true;
    }
    
    bool _showBank(Creature *creature, Player *player, uint32 offset) {
        player->PlayerTalkClass->ClearMenus();
        
        std::vector<ItemToSell> allItems;
        uint32 counter=offset;
        
        if (!player->azthPlayer->GetBankItemsList().empty()) {
            AzthPlayer::ItemInBankMap::iterator iterator = player->azthPlayer->GetBankItemsList().begin();
            if (player->azthPlayer->GetBankItemsList().size() > std::size_t(offset)) {
                if (offset>0)
                    std::advance(iterator, offset);

                for(; iterator != player->azthPlayer->GetBankItemsList().end(); iterator++) {
                    if (counter>= AZTH_ITEMBANK_RANGE+offset)
                        break;
                    
                    ItemTemplate const* _proto = sObjectMgr->GetItemTemplate(iterator->first);
                    if (!_proto)
                        continue;
                    
                    counter++;
                    
                    allItems.push_back(ItemToSell(iterator->first, _proto->Name1.c_str(), 0, true));            
                }
            }
        }

        if (allItems.empty())
        {
            player->SEND_GOSSIP_MENU(NO_ITEM_FOUND, creature->GetGUID());
        }
        else
        {
            sItemToSell->SendListInventoryDonorVendor(player->GetSession(), creature->GetGUID(), allItems, player);
        }
        
        return true;
    }


    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code) override
    {
        
        player->PlayerTalkClass->ClearMenus();
        if (action == 500 && sender == GOSSIP_SENDER_MAIN)
        {
            std::vector<ItemToSell> allItems = ItemToSellList;
            std::vector<ItemToSell> buyAbleItems;
            std::vector<ItemToSell> notBuyAbleItems;

            // get own items
            if (!player->azthPlayer->GetBankItemsList().empty()) {
                for(AzthPlayer::ItemInBankMap::iterator iterator = player->azthPlayer->GetBankItemsList().begin(); iterator != player->azthPlayer->GetBankItemsList().end(); iterator++) {
                    ItemTemplate const* _proto = sObjectMgr->GetItemTemplate(iterator->first);
                    if (!_proto)
                        continue;
                    
                    allItems.push_back(ItemToSell(iterator->first, _proto->Name1.c_str(), 0, true));            
                }
            }
            
            uint32 counter=0;
            
            // this loop search through the allItems for a compatible item name
            // but also create two lists:
            // buyableitems (to show first)
            // notbuyableitems (to show as latest)
            for (uint32 i = 0; i < allItems.size(); i++)
            {
                if (counter>=AZTH_ITEMBANK_RANGE)
                    break;
                
                string completeString = ItemToSellList[i].GetName();
                string partialString = code;
                string partialStringFirstLetterUpper = sItemToSell->CapitalizeFirstLetterEveryWord(code);
                if (completeString.find(partialString) != string::npos || completeString.find(partialStringFirstLetterUpper) != string::npos)
                {
                    if (ItemToSellList[i].GetCanBeBought())
                    {
                        buyAbleItems.push_back(ItemToSellList[i]);
                    }
                    else
                    {
                        notBuyAbleItems.push_back(ItemToSellList[i]);
                    }
                    
                    counter++;
                }
            }
            
            std::vector<ItemToSell> toSell = buyAbleItems;
            toSell.insert(buyAbleItems.end(), notBuyAbleItems.begin(), notBuyAbleItems.end());

            if (allItems.size() == 0)
            {
                player->SEND_GOSSIP_MENU(NO_ITEM_FOUND, creature->GetGUID());
            }
            else
            {
                sItemToSell->SendListInventoryDonorVendor(player->GetSession(), creature->GetGUID(), toSell, player);
            }
        }
        return true;
    }
};

class DonatorVendorPlayer : public PlayerScript
{
public:
    DonatorVendorPlayer() : PlayerScript("DonatorVendorPlayer") {}

    void OnBeforeBuyItemFromVendor(Player* player, uint64 vendorguid, uint32 vendorslot, uint32 &itemEntry, uint8 /*count*/, uint8 /*bag*/, uint8 /*slot*/)
    {
        Creature *vendor = player->GetMap()->GetCreature(vendorguid);

        if (vendor->GetScriptName() == "DonatorVendor")
        {
            uint32 guid = sItemToSell->OwnItem(player, itemEntry);

            if (guid != 0)
            {
                ItemTemplate const* _proto = sObjectMgr->GetItemTemplate(itemEntry);
                if (!_proto)
                    return;

                Item* pItem = NewItemOrBag(_proto);

                QueryResult selectItemQuery = CharacterDatabase.PQuery("SELECT creatorGuid, giftCreatorGuid, count, duration, charges, flags, enchantments, randomPropertyId, durability, playedTime, text, itemEntry FROM item_instance WHERE guid = %d", guid);
                if (selectItemQuery) {
                    Field* selectItemField = selectItemQuery->Fetch();

                    pItem->LoadFromDB(guid, player->GetGUID(), selectItemField, itemEntry);

                    ItemPosCountVec dest;

                    SQLTransaction trans = CharacterDatabase.BeginTransaction();

                    if (player->CanStoreItem(NULL_BAG, NULL_SLOT, dest, pItem, false) == EQUIP_ERR_OK) {
                        player->MoveItemToInventory(dest, pItem, true, false);
                        
                        player->SaveInventoryAndGoldToDB(trans);
                        CharacterDatabase.CommitTransaction(trans);

                        CharacterDatabase.PQuery("DELETE FROM `azth_items_bank` WHERE `guid`=%d AND `itemEntry`=%d;", player->GetGUID(), itemEntry);

                        player->azthPlayer->DelBankItem(itemEntry);

                        WorldPacket data(SMSG_BUY_ITEM, (8 + 4 + 4 + 4));
                        data << uint64(vendor->GetGUID());
                        data << uint32(vendorslot + 1);                   // numbered from 1 at client
                        data << int32(0); // new count
                        data << uint32(1); // count
                        player->GetSession()->SendPacket(&data);
                        player->SendNewItem(pItem, 1, true, false, false);
                    }
                    else {
                        ChatHandler(player->GetSession()).PSendSysMessage("Item non recuperato, unique? borse piene?");
                        sLog->outError("player can't take item back: %u", pItem->GetGUIDLow());
                    }
                } else {
                    ChatHandler(player->GetSession()).PSendSysMessage("Error: item doesn't exist in database");
                    sLog->outError("Error: item doesn't exist in database: %u", guid);
                }

                itemEntry = 0; // needed to return true from the hook
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
