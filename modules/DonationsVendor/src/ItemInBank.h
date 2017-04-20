#include "Common.h"
#include "Define.h"

class ItemInBank
{
    friend class ACE_Singleton<ItemInBank, ACE_Null_Mutex>;
public:
	//GETTERS
	uint32 GetCharacterGUID() const;
	uint32 GetItemGUID() const;
    uint32 GetItemEntry() const;
    vector<ItemInBank> GetBankItemsList() const;

	//SETTERS
    void ItemInBank::SetBankItemsList(vector<ItemInBank> itemsInBankList);

	ItemInBank();
	ItemInBank(uint32 characterGUID, uint32 itemGUID, uint32 itemEntry);

private:
	uint32 characterGUID;
	uint32 itemGUID;
    uint32 itemEntry;
    vector<ItemInBank> itemsInBankList;
};

#define sItemInBank ACE_Singleton<ItemInBank, ACE_Null_Mutex>::instance()