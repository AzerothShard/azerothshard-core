#include "AzthUtils.h"



AzthUtils::AzthUtils()
{
}


AzthUtils::~AzthUtils()
{
}


void AzthUtils::removeTimewalkingAura(Unit *unit) {
    for (Unit::AuraApplicationMap::iterator iter = unit->GetAppliedAuras().begin(); iter != unit->GetAppliedAuras().end();)
    {
        AuraApplication const* aurApp = iter->second;
        Aura const* aura = aurApp->GetBase();
        if (!aura->IsPassive()                               // don't remove passive auras
            && (aurApp->IsPositive() || unit->IsPet() || !aura->GetSpellInfo()->HasAttribute(SPELL_ATTR3_DEATH_PERSISTENT))) // not negative death persistent auras
        {
            unit->RemoveAura(iter);
        }
        // xinef: special marker, 95% sure
        else if (aura->GetSpellInfo()->HasAttribute(SPELL_ATTR5_REMOVE_ON_ARENA_ENTER))
            unit->RemoveAura(iter);
        else
            ++iter;
    }
}

float AzthUtils::getCustomMultiplier(ItemTemplate const * pProto,uint32 multiplier) {
    if (pProto->RequiredLevel >= 80) {
        return multiplier;
    }
    else {
        // ex: 131 ( boost ) / ( 80 - 60 )
        return ( (float)multiplier / float( 80 - pProto->RequiredLevel ) ) * 6;
    }
}

uint32 AzthUtils::calculateItemScalingValue(ItemTemplate const * pProto, Player *pl)
{    
    if (pl == nullptr)
        return 0;
    
    if (pProto->RequiredLevel <= pl->getLevel()) // remove / apply
        return 0;

    // SHOULDERS
    if (pProto->InventoryType == INVTYPE_SHOULDERS) { 
        uint32 mul = 8; // should be 1 ?
        if (pProto->Class == ITEM_CLASS_ARMOR) {
            switch (pProto->SubClass) {
            case ITEM_SUBCLASS_ARMOR_CLOTH :
                return mul + 32;
            case ITEM_SUBCLASS_ARMOR_LEATHER :
                return mul + 64;
            case ITEM_SUBCLASS_ARMOR_MAIL :
                return mul + 128;
            case ITEM_SUBCLASS_ARMOR_PLATE :
                return mul + 256;
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
        return 8 + 512; // should be 4 ?
    }

    // RANGED
    if (pProto->InventoryType == INVTYPE_RANGED || pProto->InventoryType == INVTYPE_AMMO || pProto->InventoryType == INVTYPE_THROWN)
        return 8 + 8192; // should be 16 ?
        
    // WANDS
    if (pProto->InventoryType == INVTYPE_RANGEDRIGHT)
        return 8 + 16384; // should be 16 ?


    // CLOAK
    if (pProto->InventoryType == INVTYPE_CLOAK) {
        return 8 + 524288;
    }
    
    // ARMOR
    if (pProto->InventoryType == INVTYPE_SHIELD) {
        return 8 + 8388608;
    }

    if (pProto->InventoryType == INVTYPE_TRINKET) {
        return 8; // should be 2 ?
    }

    if (pProto->Class == ITEM_CLASS_ARMOR) {
        uint32 mul = 8; // should be 262144 ?
        switch (pProto->SubClass) {
        case ITEM_SUBCLASS_ARMOR_CLOTH:
            return mul + 1048576;
        case ITEM_SUBCLASS_ARMOR_LEATHER:
            return mul + 2097152;
        case ITEM_SUBCLASS_ARMOR_MAIL :
            return mul + 4194304;
        case ITEM_SUBCLASS_ARMOR_PLATE:
            return mul + 8388608;
        }
    }

    return 0;

}
