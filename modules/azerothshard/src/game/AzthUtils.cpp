#include "AzthUtils.h"



AzthUtils::AzthUtils()
{
}


AzthUtils::~AzthUtils()
{
}

uint32 AzthUtils::calculateItemScalingValue(ItemTemplate const * pProto, Player *pl)
{    
    if (pl != NULL && ( pl->azthPlayer->GetTimeWalkingLevel() != NULL && pProto->RequiredLevel <= pl->azthPlayer->GetTimeWalkingLevel()))
        return 0;

    // SHOULDERS
    if (pProto->InventoryType == INVTYPE_SHOULDERS) { 
        if (pProto->Class == ITEM_CLASS_ARMOR) {
            switch (pProto->SubClass) {
            case ITEM_SUBCLASS_ARMOR_CLOTH:
                return 8 + 32;
            case ITEM_SUBCLASS_ARMOR_LEATHER :
                return 8 + 64;
            case ITEM_SUBCLASS_ARMOR_MAIL :
                return 8 + 128;
            case ITEM_SUBCLASS_ARMOR_PLATE :
                return 8 + 256;
            }
        }

        return 0;
    }

    // WEAPONS 
    if (pProto->InventoryType == INVTYPE_2HWEAPON) {
        return 8 + 1024;
    }

    if (pProto->InventoryType == INVTYPE_WEAPON || pProto->InventoryType == INVTYPE_WEAPONMAINHAND
        || pProto->InventoryType == INVTYPE_WEAPONOFFHAND) {
        return 8 + 512;
    }

    // RANGED
    if (pProto->InventoryType == INVTYPE_RANGED || pProto->InventoryType == INVTYPE_AMMO || pProto->InventoryType == INVTYPE_THROWN)
        return 8 + 8192;
        
    // WANDS
    if (pProto->InventoryType == INVTYPE_RANGEDRIGHT)
        return 8 + 16384;


    // CLOAK
    if (pProto->InventoryType == INVTYPE_CLOAK) {
        return 8 + 524288;
    }
    
    // ARMOR
    if (pProto->InventoryType == INVTYPE_SHIELD) {
        return 8 + 8388608;
    }

    if (pProto->Class == ITEM_CLASS_ARMOR) {
        switch (pProto->SubClass) {
        case ITEM_SUBCLASS_ARMOR_CLOTH:
            return 8 + 1048576;
        case ITEM_SUBCLASS_ARMOR_LEATHER:
            return 8 + 2097152;
        case ITEM_SUBCLASS_ARMOR_MAIL :
            return 8 + 4194304;
        case ITEM_SUBCLASS_ARMOR_PLATE:
            return 8 + 8388608;
        }
    }

    return 0;

}
