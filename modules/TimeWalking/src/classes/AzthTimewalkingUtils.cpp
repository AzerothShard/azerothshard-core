#include "AzthUtils.h"
#include "AzthLevelStat.h"
#include "Pet.h"

std::string AzthUtils::getLevelInfo(uint32 level) {
    switch (level) {
        case TIMEWALKING_LVL_NAXX:
            return "Naxxramas Timewalking";
        case TIMEWALKING_LVL_OBSIDIAN:
            return "Obsidian Sanctum Timewalking";
        case TIMEWALKING_LVL_THE_EYE:
            return "The Eye of Eternity Timewalking";
    }

    return std::to_string(level);
}

bool AzthUtils::updateTwLevel(Player *player,Group *group) {
    
    uint32 levelPlayer = player->azthPlayer->isTimeWalking() ? player->azthPlayer->GetTimeWalkingLevel() : player->getLevel();

    bool result = false;
    
    if (group) {
        if (group->azthGroupMgr->levelMaxGroup < levelPlayer) {
            group->azthGroupMgr->levelMaxGroup = levelPlayer;
            group->azthGroupMgr->saveToDb();
            
            std::string _slvl=sAzthUtils->getLevelInfo(levelPlayer);
            string s = "|cffff0000 Il livello di " + player->GetName() +" è stato registrato all'interno del gruppo, con il valore: " + _slvl + "|r";
            sAzthUtils->sendMessageToGroup(player, player->GetGroup(), s.c_str());
            result=true;
        }
    }

    Map* map = player->FindMap();

    if (map->IsDungeon() || map->IsRaid()) {
        bool updated=false;
        InstanceSave* is = sInstanceSaveMgr->PlayerGetInstanceSave(GUID_LOPART(player->GetGUID()), map->GetId(), player->GetDifficulty(map->IsRaid()));
        if (is->azthInstMgr->levelMax == 0) {
            player->azthPlayer->instanceID = map->GetInstanceId();

            QueryResult result = CharacterDatabase.PQuery("SELECT levelPg FROM instance WHERE id = %u", player->azthPlayer->instanceID);
            if (!result)
                return result;
            Field* fields = result->Fetch();
            is->azthInstMgr->levelMax = fields[0].GetUInt32();
            updated=true;
        }

        if (levelPlayer > is->azthInstMgr->levelMax) {
            is->azthInstMgr->levelMax = levelPlayer;
            is->InsertToDB();
            updated=true;
        }
        
        if (updated) {
            std::string _slvl = sAzthUtils->getLevelInfo(is->azthInstMgr->levelMax);
            string s = "|cffff0000 Il livello di " + player->GetName() +" è stato registrato nell'instance, con il valore: " + _slvl + "|r";
            sAzthUtils->sendMessageToGroup(player, player->GetGroup(), s.c_str());
            result=true;
        }
    }
    
    return result;
}

uint32 AzthUtils::selectSpellForTW(Player* player, uint32 spellId) {
    if (player->azthPlayer->isTimeWalking(true)) {
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

        if (aura->GetSpellInfo()->Id >= TIMEWALKING_AURA_MOD_INCREASE_HEALTH_PCT && aura->GetSpellInfo()->Id <= TIMEWALKING_AURA_VISIBLE) {
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


uint32 AzthPlayer::getTwItemLevel(uint32 twLevel) {       
    switch (twLevel) {
        case TIMEWALKING_LVL_NAXX:
        case TIMEWALKING_LVL_OBSIDIAN:
        case TIMEWALKING_LVL_THE_EYE:
            return 213;
        break;
        case TIMEWALKING_LVL_ULDUAR:
        case TIMEWALKING_LVL_TOGC:
            return 232;
        break;
    }
    
    return 0;
}
