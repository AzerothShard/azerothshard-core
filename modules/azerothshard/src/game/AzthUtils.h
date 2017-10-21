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
#include <stdio.h>
#include <time.h>

class AuraApplication;

class AzthUtils
{
    friend class ACE_Singleton<AzthUtils, ACE_Null_Mutex>;
public:
    
    std::string INVTYPE_NON_EQUIP_STRING;
    std::string INVTYPE_HEAD_STRING;
    std::string INVTYPE_NECK_STRING;
    std::string INVTYPE_SHOULDERS_STRING;
    std::string INVTYPE_BODY_STRING;
    std::string INVTYPE_CHEST_STRING;
    std::string INVTYPE_WAIST_STRING;
    std::string INVTYPE_LEGS_STRING;
    std::string INVTYPE_FEET_STRING;
    std::string INVTYPE_WRISTS_STRING;
    std::string INVTYPE_HANDS_STRING;
    std::string INVTYPE_FINGER_STRING;
    std::string INVTYPE_TRINKET_STRING;
    std::string INVTYPE_WEAPON_STRING;
    std::string INVTYPE_SHIELD_STRING;
    std::string INVTYPE_RANGED_STRING;
    std::string INVTYPE_CLOAK_STRING;
    std::string INVTYPE_2HWEAPON_STRING;
    std::string INVTYPE_BAG_STRING;
    std::string INVTYPE_TABARD_STRING;
    std::string INVTYPE_ROBE_STRING;
    std::string INVTYPE_WEAPONMAINHAND_STRING;
    std::string INVTYPE_WEAPONOFFHAND_STRING;
    std::string INVTYPE_HOLDABLE_STRING;
    std::string INVTYPE_AMMO_STRING;
    std::string INVTYPE_THROWN_STRING;
    std::string INVTYPE_RANGEDRIGHT_STRING;
    std::string INVTYPE_QUIVER_STRING;
    std::string INVTYPE_RELIC_STRING;
    
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
    
    time_t getStartsOfYear();
    
    // horde version of objectmgr factionchangeitems map
    ObjectMgr::CharacterConversionMap FactionChangeItemsHorde;
    
    std::list<uint32> startSpells[MAX_RACES][MAX_CLASSES];
    
    std::string GetItemIcon(uint32 entry, uint32 width, uint32 height, int x, int y);
    std::vector<std::string> getCategoryIconAndNameByItemType(uint32 itemType);

};

#define sAzthUtils ACE_Singleton<AzthUtils, ACE_Null_Mutex>::instance()

#endif