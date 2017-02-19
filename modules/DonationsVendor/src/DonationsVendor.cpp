#include "ScriptMgr.h"
#include "Player.h"
#include "Item.h"
#include "ItemToSell.h"
#include "ConditionMgr.h"

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
public:
    DonatorVendor() : CreatureScript("DonatorVendor") {}

    bool OnGossipHello(Player* player, Creature* creature)
    {
        //                              icon            text                           sender      action   popup message    money  code
        player->ADD_GOSSIP_ITEM_EXTENDED(0, "Inserisci gli item da cercare qui", GOSSIP_SENDER_MAIN, 1, "Inserisci nome item", 0, true);
        player->SEND_GOSSIP_MENU(1, creature->GetGUID());
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
                player->SEND_GOSSIP_MENU(NO_ITEM_FOUND, creature->GetGUID()); //mandare menu con scritta "nessun risultato trovato"
            }
            else
            {
                sItemToSell->SendListInventoryDonorVendor(player->GetSession(), creature->GetGUID(), allItems);
            }
        }
        return true;
    }
};

void AddSC_DonationsVendor()
{
    new loadItemVendor();
    new DonatorVendor();
}