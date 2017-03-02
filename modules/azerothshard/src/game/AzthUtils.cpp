#include "AzthUtils.h"
#include "AzthLevelStat.h"
#include "Pet.h"



AzthUtils::AzthUtils()
{
}


AzthUtils::~AzthUtils()
{
}

uint32 AzthUtils::selectCorrectSpellRank(uint8 level, uint32 spellId) {
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);

    for (SpellInfo const* nextSpellInfo = spellInfo; nextSpellInfo != NULL; nextSpellInfo = nextSpellInfo->GetPrevRankSpell())
    {
        if ((nextSpellInfo->SpellLevel == 0 && uint32(level) >= nextSpellInfo->BaseLevel) || uint32(level) >= nextSpellInfo->SpellLevel)
                return nextSpellInfo->Id;
    }

    return 0;
}

uint32 AzthUtils::selectSpellForTW(Player* player, uint32 spellId) {
    if (player->azthPlayer->GetTimeWalkingLevel() > 0) {
        uint32 spell=this->selectCorrectSpellRank(player->getLevel(), spellId);
        if (spell)
            return spell;
    }

    return spellId;
}

void AzthUtils::removeTimewalkingAura(Unit *unit) {
    
    std::list<uint32> spells;

    for (Unit::AuraApplicationMap::iterator iter = unit->GetAppliedAuras().begin(); iter != unit->GetAppliedAuras().end();)
    {
        AuraApplication const* aurApp = iter->second;
        Aura const* aura = aurApp->GetBase();

        // we remove/apply them in another place
        if (aura->GetSpellInfo()->Id >= TIMEWALKING_AURA_MOD_HEALING && aura->GetSpellInfo()->Id <= TIMEWALKING_AURA_VISIBLE) {
            ++iter;
            continue;
        }

        if (aurApp->IsPositive() || unit->IsPet() || !aura->GetSpellInfo()->HasAttribute(SPELL_ATTR3_DEATH_PERSISTENT)) // not negative death persistent auras
        {
            unit->RemoveAura(iter);
        }
        // xinef: special marker, 95% sure
        else if (aura->GetSpellInfo()->HasAttribute(SPELL_ATTR5_REMOVE_ON_ARENA_ENTER))
            unit->RemoveAura(iter);
        else {
            ++iter;
        }
    }

    if (unit->GetTypeId() == TYPEID_PLAYER) {
        Player *pl = (Player*)unit;

        // xinef: add talent auras
        for (PlayerTalentMap::const_iterator itr = pl->GetTalentMap().begin(); itr != pl->GetTalentMap().end(); ++itr)
        {
            if (itr->second->State == PLAYERSPELL_REMOVED)
                continue;

            // xinef: talent not in new spec
            if (!(itr->second->specMask & pl->GetActiveSpecMask()))
                continue;

            spells.push_back(itr->first);
        }

        for (PlayerSpellMap::iterator itr = pl->GetSpellMap().begin(); itr != pl->GetSpellMap().end();) {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(itr->first);
            if (spellInfo->IsPassive() && spellInfo->IsPositive()) {
                spells.push_back(itr->first);
            }
            ++itr;
        }

        for (uint8 i = 0; i < MAX_GLYPH_SLOT_INDEX; ++i)
        {
            if (uint32 glyph = pl->GetGlyph(i))
            {
                if (GlyphPropertiesEntry const* glyphEntry = sGlyphPropertiesStore.LookupEntry(glyph))
                {
                    if (GlyphSlotEntry const* glyphSlotEntry = sGlyphSlotStore.LookupEntry(pl->GetGlyphSlot(i)))
                    {
                        const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(glyphEntry->SpellId);
                        if (glyphEntry->TypeFlags == glyphSlotEntry->TypeFlags)
                        {
                            if (!spellInfo->Stances)
                                spells.push_back(glyphEntry->SpellId);
                            continue;
                        }
                    }
                }
            }
        }

        for (std::list<uint32>::const_iterator iterator = spells.begin(), end = spells.end(); iterator != end; ++iterator) {
            pl->RemoveAurasDueToSpell((*iterator));
            const SpellInfo *spellInfo = sSpellMgr->GetSpellInfo((*iterator));

            if (spellInfo->IsPassive() || (spellInfo->HasAttribute(SPELL_ATTR0_HIDDEN_CLIENTSIDE) && spellInfo->Stances))
            {
                if (pl->IsNeedCastPassiveSpellAtLearn(spellInfo))
                    pl->CastSpell(pl, (*iterator), true);
            }
            // pussywizard: cast and return, learnt spells will update profession count, etc.
            else if (spellInfo->HasEffect(SPELL_EFFECT_SKILL_STEP))
            {
                pl->CastSpell(pl, (*iterator), true);
            }
        }

        pl->_ApplyAllLevelScaleItemMods(false);
        pl->_ApplyAllLevelScaleItemMods(true);
        
        pl->InitStatsForLevel(true);
    }
    else if (unit->IsPet()) {
        Pet *pet = (Pet*)unit;

        for (PetSpellMap::iterator itr = pet->m_spells.begin(); itr != pet->m_spells.end();) {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(itr->first);
            if (spellInfo->IsPassive() && spellInfo->IsPositive()) {
                spells.push_back(itr->first);
            }
            ++itr;
        }

        for (std::list<uint32>::const_iterator iterator = spells.begin(), end = spells.end(); iterator != end; ++iterator) {
            pet->RemoveAurasDueToSpell((*iterator));
            pet->CastSpell(pet, (*iterator), true);
        }
    }

}

uint32 AzthUtils::getCalcReqLevel(ItemTemplate const* pProto) {
    if (pProto->RequiredLevel <= 1 && pProto->ItemLevel >= 6) { // zones >= lvl 5 
        // classic rare and lower
        if (pProto->ItemLevel <= 60)
            return pProto->ItemLevel;
        
        // classic epic
        if (pProto->ItemLevel > 60 && pProto->ItemLevel <= 92 && pProto->Quality >= ITEM_QUALITY_EPIC) {
            return 60;
        }
        
        // tbc low level items
        if (pProto->ItemLevel > 81 && pProto->ItemLevel <= 92) {
            return 60;
        }
        
        // tbc rare and lower
        if (pProto->ItemLevel > 92 && pProto->ItemLevel <= 115) {
            return ((pProto->ItemLevel) - 92 / 2 ) +1;
        }
            
        // tbc epic
        if (pProto->ItemLevel > 115 && pProto->ItemLevel <= 164 && pProto->Quality >= ITEM_QUALITY_EPIC) {
            return ((pProto->ItemLevel) - 130 / 3 ) + 1;
        }
        
        //if (pProto->ItemLevel > 130 && pProto->ItemLevel <= 284)
        //    return 80;
        
        // 130 - 284
        return sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
    }

    return pProto->RequiredLevel;
}

float AzthUtils::getCustomMultiplier(ItemTemplate const * pProto,uint32 multiplier) {
    uint32 req=getCalcReqLevel(pProto);
    uint32 maxLevel = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
    if (req >= maxLevel) {
        return multiplier;
    }
    else {
        // ex: 131 ( boost ) / ( 80 - 60 )
        return ( (float)multiplier / float( maxLevel - req ) ) * 6;
    }
}

uint32 AzthUtils::calculateItemScalingValue(ItemTemplate const * pProto, Player *pl)
{    
    if (pl == nullptr)
        return 0;
    
    uint32 req=getCalcReqLevel(pProto);
    if (req <= pl->getLevel()) // remove / apply
        return 0;

    //uint32 mul = 4; // should be 1 ?

    // SHOULDERS
    if (pProto->InventoryType == INVTYPE_SHOULDERS) { 
        if (pProto->Class == ITEM_CLASS_ARMOR) {
            switch (pProto->SubClass) {
            case ITEM_SUBCLASS_ARMOR_CLOTH :
                return 16 + 32; //return 1 + 32;
            case ITEM_SUBCLASS_ARMOR_LEATHER :
                return 16 + 64; //return 1 + 64;
            case ITEM_SUBCLASS_ARMOR_MAIL :
                return 16 + 128; //return 1 + 128;
            case ITEM_SUBCLASS_ARMOR_PLATE :
                return 16 + 128; //return 1 + 256;
            }
        }

        return 0;
    }
    
        
    if (pProto->InventoryType == INVTYPE_CHEST || pProto->InventoryType == INVTYPE_ROBE) {
        switch (pProto->SubClass) {
        case ITEM_SUBCLASS_ARMOR_CLOTH:
            return 16 + 1048576; //return 8 + 1048576;
        case ITEM_SUBCLASS_ARMOR_LEATHER:
            return 16 + 2097152; //return 8 + 2097152;
        case ITEM_SUBCLASS_ARMOR_MAIL :
            return 16 + 4194304; // return 8 + 4194304;
        case ITEM_SUBCLASS_ARMOR_PLATE:
            return 16 + 8388608;  //return 8 + 8388608;
        }
        
        return 0;
    }

    // WEAPONS 
    if (pProto->InventoryType == INVTYPE_2HWEAPON) {
        if (pProto->Class == ITEM_CLASS_WEAPON && pProto->SubClass == ITEM_SUBCLASS_WEAPON_STAFF ) {
            return 16; // + 4096 + 32768; // return 8 + 4096 + 32768;
        } else {
            return 16; // + 1024; //return 8 + 1024;
        }
    }

    if (pProto->InventoryType == INVTYPE_WEAPON || pProto->InventoryType == INVTYPE_WEAPONMAINHAND
        || pProto->InventoryType == INVTYPE_WEAPONOFFHAND) {
        return 16; //+ 512; //return 4 + 512; // should be 4 ?
    }

    // RANGED
    if (pProto->InventoryType == INVTYPE_RANGED || pProto->InventoryType == INVTYPE_AMMO || pProto->InventoryType == INVTYPE_THROWN)
        return 16; //+ 8192; // should be 16 ?
        
    // WANDS
    if (pProto->InventoryType == INVTYPE_RANGEDRIGHT)
        return 16; //+ 16384; // should be 16 ?


    if (pProto->InventoryType == INVTYPE_TRINKET) {
        return 2; // should be 2 ?
    }

   
    if (pProto->InventoryType == INVTYPE_FINGER)
        return 16; // return 262144;
    
    
    if (pProto->InventoryType == INVTYPE_NECK)
        return 16; //return 262144;
    
    //
    // special unknown cases
    //

    if (pProto->InventoryType == INVTYPE_HOLDABLE || pProto->InventoryType == INVTYPE_RELIC) {
        return 16; //return 4 + 512;
    }
    
    // CLOAK
    if (pProto->InventoryType == INVTYPE_CLOAK) {
        return 16 + 524288; //return 4 + 524288;
    }
    
    // ARMOR
    if (pProto->InventoryType == INVTYPE_SHIELD) {
        return 16 + 8388608; // return 4 + 8388608;
    }
    
    if (pProto->Class == ITEM_CLASS_ARMOR) {
        switch (pProto->SubClass) {
        case ITEM_SUBCLASS_ARMOR_CLOTH:
            return 16 + 1048576; // return 4 + 1048576;
        case ITEM_SUBCLASS_ARMOR_LEATHER:
            return 16 + 2097152; //return 4 + 2097152;
        case ITEM_SUBCLASS_ARMOR_MAIL :
            return 16 + 4194304; //return 4 + 4194304;
        case ITEM_SUBCLASS_ARMOR_PLATE:
            return 16 + 8388608; //return 4 + 8388608;
        }
    }

    return 0;

}
