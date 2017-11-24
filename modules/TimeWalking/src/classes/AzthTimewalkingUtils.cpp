#include "AzthUtils.h"
#include "AzthLevelStat.h"
#include "Pet.h"
#include "Spell.h"

class Spell;

std::string AzthUtils::getLevelInfo(uint32 level) {
    switch (level) {
        case TIMEWALKING_LVL_NAXX:
            return "Naxxramas Timewalking";
        case TIMEWALKING_LVL_OBSIDIAN:
            return "Obsidian Sanctum Timewalking";
        case TIMEWALKING_LVL_THE_EYE:
            return "The Eye of Eternity Timewalking";
        case TIMEWALKING_LVL_TOGC:
            return "Trial of the Grand Crusader";
        case TIMEWALKING_LVL_ULDUAR:
            return "Ulduar";
        case TIMEWALKING_LVL_AUTO:
            return "Auto scaling";
    }

    return std::to_string(level);
}

bool AzthUtils::isEligibleForBonusByArea(Player const* player) {
    /*WorldLocation pos = WorldLocation(player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());
    uint32 posLvl=sAzthUtils->getPositionLevel(true, player->GetMap(), pos);
    
    if (posLvl>0) {
        uint32 groupLevel = player->azthPlayer->getGroupLevel();
        uint32 level = groupLevel > 0 ? groupLevel : player->getLevel();
        
        if (posLvl>TIMEWALKING_SPECIAL_LVL_MIN) {
            if (player->azthPlayer->GetTimeWalkingLevel() == posLvl) // with special level we must have the exact value
                return true;
        } else if (
            level != sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)  // avoid double loot and other bonuses on end-game instances
            && level<=posLvl+3  //TODO: replace fixed value with config
        ) {
            return true;
        }
    } */
    
    return false;
}

bool AzthUtils::updateTwLevel(Player *player,Group *group) {
    bool result = false;
    
    if (!player || !player->azthPlayer)
        return result;
    
    uint32 levelPlayer = player->azthPlayer->isTimeWalking() && player->azthPlayer->GetTimeWalkingLevel() != TIMEWALKING_LVL_AUTO 
                                ? player->azthPlayer->GetTimeWalkingLevel() : player->getLevel();
    
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
        if (is) { // negative condition shouldn't happen (maybe only on login?)
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

    for (Unit::AuraApplicationMap::iterator iter = unit->GetAppliedAuras().begin(); iter != unit->GetAppliedAuras().end(); iter++)
    {
        AuraApplication const* aurApp = iter->second;
        Aura const* aura = aurApp->GetBase();

        if (aura->GetSpellInfo()->Id >= TIMEWALKING_AURA_MIN && aura->GetSpellInfo()->Id <= TIMEWALKING_AURA_MAX) {
            continue;
        }

        if (aurApp->IsPositive() || unit->IsPet() || !aura->GetSpellInfo()->HasAttribute(SPELL_ATTR3_DEATH_PERSISTENT)) // not negative death persistent auras
        {
            // keep auras that are lower or equal to unit level
            if (aura->GetSpellInfo()->SpellLevel <= unit->getLevel())
                continue;
            
            unit->RemoveAura(iter);
        }
        // xinef: special marker, 95% sure
        else if (aura->GetSpellInfo()->HasAttribute(SPELL_ATTR5_REMOVE_ON_ARENA_ENTER)) 
        {
            // keep auras that are lower or equal to unit level
            if (aura->GetSpellInfo()->SpellLevel <= unit->getLevel())
                continue;
            
            unit->RemoveAura(iter);
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

int32 AzthUtils::getSpellReduction(Player *player, SpellInfo const* spellProto) {    
    // special cases where we must consider valid spell
    // even if no level (e.g. custom spell triggered by scripts)
    switch(spellProto->Id) {
        case 20647: // EXECUTE
            return 0;
    }
    
    // we need to use damage/heal reduction from TW table when in TW 
    // at 80 special levels
    uint32 twLevel=player->azthPlayer->GetTimeWalkingLevel();
    if (twLevel >= TIMEWALKING_SPECIAL_LVL_MIN && twLevel <= TIMEWALKING_SPECIAL_LVL_MAX)
        return -1;
    
    uint32 spellLevel = spellProto->SpellLevel == 0 ? spellProto->BaseLevel : spellProto->SpellLevel;

    if (spellLevel <= 1 && spellProto->MaxLevel == 0 && spellProto->GetNextRankSpell() == NULL)
        spellLevel = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL); // assume that spell is max level
        //return -1; // not valid and not scalable. Use reduction pct from TW table
    
    // proportional reduction with ranked spells
    if (spellLevel > player->getLevel()) {

        if (spellLevel>70) {
            // end-game spell must be reduced more 
            // 1 level from 70 to 80 is comparable to 2 level from 1 to 70
            // after a calculation via graph based on some spell coefficient
            spellLevel=70+((spellLevel-70)*2);
        }
        
        uint8 diff = uint8(spellLevel - player->getLevel());
        int32 pct = ceil((diff*100)/sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL));

        return pct > 100 ? 99 : pct;        
    }

    
    return 0; // no reduction, but scaled
}

bool AzthUtils::isValidSpellForTw(SpellInfo const* spellProto) {
    switch(spellProto->Id) {
        case 54428: // Divine Plea
        //case 53601: // Sacred Shield
            return true;
    }
    
    return false;
}