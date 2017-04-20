#include "ItemInBank.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"


ItemInBank::ItemInBank()
{
    characterGUID = uint32(0);
    itemGUID = uint32(0);
    itemEntry = uint32(0);
}


ItemInBank::ItemInBank(uint32 characterGUID, uint32 itemGUID, uint32 itemEntry)
{
	this->characterGUID = characterGUID;
    this->itemGUID = itemGUID;
    this->itemEntry = itemEntry;
}

uint32 ItemInBank::GetCharacterGUID() const
{
	return characterGUID;
}

uint32 ItemInBank::GetItemGUID() const
{
	return itemGUID;
}

uint32 ItemInBank::GetItemEntry() const
{
    return itemEntry;
}

void ItemInBank::SetBankItemsList(vector<ItemInBank> itemsInBankList)
{
    this->itemsInBankList = itemsInBankList;
}

vector<ItemInBank> ItemInBank::GetBankItemsList() const
{
    return itemsInBankList;
}