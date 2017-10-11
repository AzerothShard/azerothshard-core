#include "ItemToSell.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Creature.h"
#include "ObjectMgr.h"
#include "Player.h"

class Player;

ItemToSell::ItemToSell()
{
	id = uint32(0);
    name = string("");
	extCost = uint32(0);
    canBeBought = bool(false);
}

ItemToSell::ItemToSell(uint32 ItsId, string ItsName, uint32 ItsExtCost, bool ItsCanBeBought)
{
	id = ItsId;
    name = ItsName;
    extCost = ItsExtCost;
    canBeBought = ItsCanBeBought;
}

string ItemToSell::GetName() const
{
	return name;
}


void ItemToSell::SetName(string ItsName)
{
	name = ItsName;
}

uint32 ItemToSell::GetId() const
{
	return id;
}


void ItemToSell::SetId(uint32 ItsId)
{
	id = ItsId;
}

uint32 ItemToSell::GetExtCost() const
{
	return extCost;
}

void ItemToSell::SetExtCost(uint32 ItsExtCost)
{
	extCost = ItsExtCost;
}

bool ItemToSell::GetCanBeBought() const
{
    return canBeBought;
}

void ItemToSell::SetCanBeBought(bool ItsCanBeBought)
{
    canBeBought = ItsCanBeBought;
}

void ItemToSell::SendListInventoryDonorVendor(WorldSession *session, uint64 vendorGuid, std::vector<ItemToSell> allItems, Player* player) {

    uint32 itemCount = allItems.size();
    uint8 count = 0;

    WorldPacket data(SMSG_LIST_INVENTORY, 8 + 1 + itemCount * 8 * 4);
    data << vendorGuid;

    size_t countPos = data.wpos();
    data << uint8(count);

    if (itemCount > MAX_VENDOR_ITEMS)
       itemCount = MAX_VENDOR_ITEMS;


    for (uint32 slot = 0; slot < itemCount; ++slot) {
        ItemTemplate const* _proto = sObjectMgr->GetItemTemplate(allItems[slot].GetId());
        
        uint32 extCost = allItems[slot].GetExtCost();
        if (_proto)
        {
            bool own = OwnItem(player, allItems[slot].GetId());
            
            data << uint32(slot + 1); // client expects counting to start at 1
            data << uint32(allItems[slot].GetId());
            data << uint32(_proto->DisplayInfoID);
            if (own)
                data << int32(1); // allow to buy 1 item at time for own items (collection)
            else if (allItems[slot].GetCanBeBought())
                data << int32(0xFFFFFFFF);
            else
                data << int32(0);
            data << uint32(0);
            data << uint32(_proto->MaxDurability);
            data << uint32(_proto->BuyCount);
            data << uint32(extCost);
        }
        if (++count >= MAX_VENDOR_ITEMS)
            break;
    }


    if (count == 0) {
        data << uint8(0);
        session->SendPacket(&data);
        return;
    }

    data.put<uint8>(countPos, count);
    session->SendPacket(&data);
}

uint32 ItemToSell::OwnItem(Player *player, uint32 itemId)
{
    uint32 guid = 0;

    if (!player->azthPlayer->GetBankItemsList().empty()) {
        auto item=player->azthPlayer->GetBankItemsList().find(itemId);
        if (item != player->azthPlayer->GetBankItemsList().end())
            guid = item->second;
    }

    return guid;
}

string ItemToSell::CapitalizeFirstLetterEveryWord(string str)
{
    bool check;
    check = false;
    for (std::size_t i = 0; i<str.length(); i++)
    {
        if (check == false && (str.at(i) >= 'a' && str.at(i) <= 'z'))//check if its a new word. 
            str.at(i) = str.at(i) + 'A' - 'a';

        if ((str.at(i) >= 'a' && str.at(i) <= 'z') || (str.at(i) >= 'A' && str.at(i) <= 'Z'))//you can also change this condition to check for a space in the string
            check = true;							//if(str.at(i)!=' ')		
        else 									//true... if character at i is an alphabet and false if not
            check = false;							//it reiterates and changes the case of the next character depending on the condition
    }
    return str;
}