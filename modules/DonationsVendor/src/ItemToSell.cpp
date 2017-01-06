#include "ItemToSell.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"

enum npc_donationsVendor_enum
{
    NPC_DONATIONS_VENDOR_ID = 300206,
};


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

void ItemToSell::SendListInventory(WorldSession *session, uint64 vendorGuid, std::vector<ItemToSell> buyableItems) {

    uint8 itemCount = buyableItems.size();
    uint8 count = 0;

    WorldPacket data(SMSG_LIST_INVENTORY, 8 + 1 + itemCount * 8 * 4);
    data << vendorGuid;

    size_t countPos = data.wpos();
    data << uint8(count);

    for (uint8 slot = 0; slot < itemCount; ++slot) {
        ItemTemplate const* _proto = sObjectMgr->GetItemTemplate(buyableItems[slot].GetId());
        
        uint32 extCost = buyableItems[slot].GetExtCost();
        if (_proto)
        {
            data << uint32(slot + 1); // client expects counting to start at 1
            data << uint32(buyableItems[slot].GetId());
            data << uint32(_proto->DisplayInfoID);
            if (buyableItems[slot].GetCanBeBought())
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

string ItemToSell::CapitalizeFirstLetterEveryWord(string str)
{
    bool check;
    check = false;
    for (int i = 0; i<str.length(); i++)
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