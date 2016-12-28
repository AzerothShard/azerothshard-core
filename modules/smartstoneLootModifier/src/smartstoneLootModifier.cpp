#include "ScriptMgr.h"
#include "Player.h"
#include "Item.h"
#include "ItemPrototype.h"
#include "Common.h"

class lootModifier : public GlobalScript
{
public:
	lootModifier() : GlobalScript("lootModifier") {}
    
	void OnBeforeDropAddItem(Player const* player, Loot& loot, LootStoreItem* lootStore)
	{
		if (player->azthPlayer->hasSmartStoneCommand(7))
        {
			ItemTemplate const* _proto = sObjectMgr->GetItemTemplate(lootStore->itemid);
			if (!_proto)
				return;
			if (_proto->Class == 7 && _proto->SubClass == 9) //class: 7, subclass: 9 ->herbalism
				for (uint32 i = 0; i < 1; i++)
					loot.AddItem(*lootStore);
            }
            else if (player->azthPlayer->hasSmartStoneCommand(8))
            {

			ItemTemplate const* _proto = sObjectMgr->GetItemTemplate(lootStore->itemid);
			if (!_proto)
				return;
			if (_proto->Class == 7 && _proto->SubClass == 7) //class: 7, subclass: 7 ->mining
				for(uint32 i = 0; i < 100; i++)
					loot.AddItem(*lootStore);
            }
	}
};

void AddSC_smartstoneLootModifier()
{
    new lootModifier();
}