#ifndef DONATIONSVENDOR_H
#define DONATIONSVENDOR_H

#include "Common.h"
#include "Define.h"

class ItemToSell
{
public:
	//GETTERS
	string GetName() const;
	uint32 GetId() const;
	uint32 GetExtCost() const;
    bool GetCanBeBought() const;

	//SETTERS
	void SetName(string name);
	void SetId(uint32 id);
	void SetExtCost(uint32 extCost);
    void SetCanBeBought(bool canBeBought);

	ItemToSell();
	ItemToSell(uint32 id, string name, uint32 extCost, bool canBeBought);
    void SendListInventory(WorldSession * session, uint64 vendorGuid, std::vector<ItemToSell> buyableItems);
    string CapitalizeFirstLetterEveryWord(string str);

private:
	string name;
	uint32 id;
	uint32 extCost;
    bool canBeBought;
};

#define sItemToSell ACE_Singleton<ItemToSell, ACE_Null_Mutex>::instance()

#endif