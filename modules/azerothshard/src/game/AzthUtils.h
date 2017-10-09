#ifndef AZTH_UTILS_H
#define	AZTH_UTILS_H

#include "Common.h"
#include "Define.h"
#include "Config.h"
#include "Item.h"
#include "ItemPrototype.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "SpellAuras.h"

class AuraApplication;

class AzthUtils
{
    friend class ACE_Singleton<AzthUtils, ACE_Null_Mutex>;
public:
    AzthUtils();
    ~AzthUtils();

    uint32 selectCorrectSpellRank(uint8 level, uint32 spellId);

    uint32 selectSpellForTW(Player* player, uint32 spellId);

    void removeTimewalkingAura(Unit * unit);

    float getCustomMultiplier(ItemTemplate const * pProto, uint32 multiplier);

    uint32 getCalcReqLevel(ItemTemplate const* pProto);

    uint32 calculateItemScalingValue(ItemTemplate const* pProto,Player *pl = NULL);
    
    void learnClassSpells(Player* player, bool new_level);
    
    void loadClassSpells();
    
    // horde version of objectmgr factionchangeitems map
    ObjectMgr::CharacterConversionMap FactionChangeItemsHorde;
    
    std::list<uint32> startSpells[MAX_RACES][MAX_CLASSES];
};

#define sAzthUtils ACE_Singleton<AzthUtils, ACE_Null_Mutex>::instance()

std::string GetItemIcon(uint32 entry, uint32 width, uint32 height, int x, int y);
std::vector<std::string> getCategoryIconAndNameByItemType(uint32 itemType);


#endif