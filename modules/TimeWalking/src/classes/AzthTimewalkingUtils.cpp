#include "AzthUtils.h"
#include "AzthLevelStat.h"
#include "Pet.h"

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