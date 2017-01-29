#ifndef DONATIONSVENDOR_H
#define DONATIONSVENDOR_H

#include "Common.h"
#include "Define.h"

enum npc_donationsVendor_enum
{
    NO_ITEM_FOUND = 50003,
    NPC_DONATIONS_VENDOR_ID = 300206,
};

class ItemToSell
{
    friend class ACE_Singleton<ItemToSell, ACE_Null_Mutex>;
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
    void SendListInventoryDonorVendor(WorldSession * session, uint64 vendorGuid, std::vector<ItemToSell> buyableItems);
    string CapitalizeFirstLetterEveryWord(string str);

private:
	string name;
	uint32 id;
	uint32 extCost;
    bool canBeBought;
};

#define sItemToSell ACE_Singleton<ItemToSell, ACE_Null_Mutex>::instance()

#endif