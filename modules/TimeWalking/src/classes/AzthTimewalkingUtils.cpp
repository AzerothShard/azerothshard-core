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

uint32 AzthUtils::maxTwLevel(uint32 sourceLvl, uint32 compareLevel) const {
    uint32 maxLevel=sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
    if (sourceLvl<=maxLevel && compareLevel <=maxLevel) {
        return sourceLvl > compareLevel ? sourceLvl : compareLevel;
    }
    
    if (sourceLvl >= TIMEWALKING_SPECIAL_LVL_MAX_START && sourceLvl <= TIMEWALKING_SPECIAL_LVL_MAX_END) {
        if (compareLevel  >= TIMEWALKING_SPECIAL_LVL_MAX_START && compareLevel <= TIMEWALKING_SPECIAL_LVL_MAX_END) {
            return sourceLvl > compareLevel ? sourceLvl : compareLevel;
        }
        
        // if sourceLevel is a special level but
        // compare level is not, then we must check about maxlevel
        // since special levels are considered "between" maxlevel -1 and maxlevel
        return compareLevel >= maxLevel ? compareLevel : sourceLvl; 
    }
    
    return maxTwLevel(compareLevel, sourceLvl); // inverse case
}

bool AzthUtils::isEligibleForBonusByArea(Player const* player) {   
    WorldLocation pos = WorldLocation(player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());
    uint32 posLvl=sAzthUtils->getPositionLevel(true, player->GetMap(), pos);
    
    if (posLvl>0) {
        uint32 groupLevel = player->azthPlayer->getGroupLevel();
        uint32 level = groupLevel > 0 ? groupLevel : player->getLevel();
        
        if (posLvl>TIMEWALKING_SPECIAL_LVL_MIN) {
            if (player->azthPlayer->GetTimeWalkingLevel() == posLvl) // with special level we must have the exact value
                return true;
        } else if (
            level != sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) && posLvl != sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) // avoid double loot and other bonuses on end-game instances
            && level<=posLvl
        ) {
            return true;
        }
    }
    
    return false;
}

// used for pet and players
void AzthUtils::setTwAuras(Unit *unit, AzthLevelStat const *stats, bool apply) {
    if (apply) {
        for ( auto& a : stats->pctMap) {
            if (a.first != TIMEWALKING_AURA_VISIBLE)
                unit->SetAuraStack(a.first, unit, a.second);
        }

        unit->SetAuraStack(TIMEWALKING_AURA_VISIBLE, unit, stats->GetLevel());
    } else {     
        for ( auto a : stats->pctMap) {
            unit->RemoveAura(a.first);
        }

        unit->RemoveAura(TIMEWALKING_AURA_VISIBLE);
    }
}

AzthLevelStat const* AzthUtils::getTwStats(Player *player, uint32 level) {
    std::map<uint32, AzthLevelStat>::const_iterator itr = sAzthLevelStatMgr->GetLevelStatList().find(level * 10000 + player->getRace() * 100 + player->getClass());
    
    if (itr == sAzthLevelStatMgr->GetLevelStatList().end())
        return nullptr;
    
    return &itr->second;
}

bool AzthUtils::updateTwLevel(Player *player,Group *group) {
    bool result = false;
    
    if (!player || !player->azthPlayer)
        return result;
    
    uint32 levelPlayer = player->azthPlayer->isTimeWalking() && player->azthPlayer->GetTimeWalkingLevel() != TIMEWALKING_LVL_AUTO 
                                ? player->azthPlayer->GetTimeWalkingLevel() : player->getLevel();
    
    if (group) {
        if (sAzthUtils->maxTwLevel(levelPlayer, group->azthGroupMgr->levelMaxGroup) != group->azthGroupMgr->levelMaxGroup) {
            group->azthGroupMgr->levelMaxGroup = levelPlayer;
            group->azthGroupMgr->saveToDb();
            
            std::string _slvl=sAzthUtils->getLevelInfo(levelPlayer);
            std::string msg = sAzthLang->getf(AZTH_LANG_GROUP_LEVEL_REG,player, player->GetName().c_str(),_slvl.c_str());
            sAzthUtils->sendMessageToGroup(player, player->GetGroup(), msg.c_str());
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

            if (sAzthUtils->maxTwLevel(levelPlayer, is->azthInstMgr->levelMax) != is->azthInstMgr->levelMax) {
                is->azthInstMgr->levelMax = levelPlayer;
                is->InsertToDB();
                updated=true;
            }
        
            if (updated) {
                std::string _slvl = sAzthUtils->getLevelInfo(is->azthInstMgr->levelMax);
                std::string msg=sAzthLang->getf(AZTH_LANG_INSTANCE_LEVEL_REG, player, player->GetName().c_str(),_slvl.c_str());
                sAzthUtils->sendMessageToGroup(player, player->GetGroup(), msg.c_str());
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

        uint32 spellLevel = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
        // not not allowed -> allowed
        if (!isNotAllowedSpellForTw(aura->GetSpellInfo())) {
            spellLevel = aura->GetSpellInfo()->SpellLevel == 0 ? aura->GetSpellInfo()->BaseLevel : aura->GetSpellInfo()->SpellLevel;
        }
        
        // item exploit check
        if (unit->GetTypeId() == TYPEID_PLAYER) {
            Player *pl = (Player*)unit;
            ItemTemplate const *proto = NULL;
            Item* castItem = aura->GetCastItemGUID() ? pl->GetItemByGuid(aura->GetCastItemGUID()) : NULL;
            if (!castItem && aura->GetCastItemEntry()) {
                proto = sObjectMgr->GetItemTemplate(aura->GetCastItemEntry());
            } else if (castItem) {
                proto = castItem->GetTemplate();
            }

            if (proto) {
                uint32 reqLevel=getCalcReqLevel(proto);
                if (reqLevel > spellLevel) {
                    spellLevel = reqLevel;
                }
            }
        }

        if (aurApp->IsPositive() || unit->IsPet() || !aura->GetSpellInfo()->HasAttribute(SPELL_ATTR3_DEATH_PERSISTENT)) // not negative death persistent auras
        {
            // keep auras that are lower or equal to unit level
            if (spellLevel > 1 && spellLevel <= unit->getLevel())
                continue;

            unit->RemoveAura(iter);
        }
        // xinef: special marker, 95% sure
        else if (aura->GetSpellInfo()->HasAttribute(SPELL_ATTR5_REMOVE_ON_ARENA_ENTER)) 
        {
            // keep auras that are lower or equal to unit level
            if (spellLevel > 1 && spellLevel <= unit->getLevel())
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
        
        //pl->InitStatsForLevel(true);
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
    uint32 lastSpell=sSpellMgr->GetLastSpellInChain(spellProto->Id);
    // special cases where we must consider valid spell
    // even if no level (e.g. custom spell triggered by scripts)
    switch(lastSpell) {
        case 20647: // EXECUTE   (triggered)
        case 58381: // mind flay (triggered)
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
        
    // apply a default reduction from 30% (lvl 1) to 6% (lvl 80) 
    int32 baseReduction = 30 - ceil((player->getLevel()/10) * 3);
    
    // proportional reduction with ranked spells
    if (spellLevel > player->getLevel()) {

        if (spellLevel>70) {
            // end-game spell must be reduced more 
            // 1 level from 70 to 80 is comparable to 2 level from 1 to 70
            // after a calculation via graph based on some spell coefficient
            spellLevel=70+((spellLevel-70)*2);
        } else if (spellLevel>50) {
            // classic end-game and tbc spells must be reduced  
            // too (we can use a different formula eventually, test needed)
            spellLevel=ceil(50+((spellLevel-50)*2));
        }
        
        uint8 diff = uint8(spellLevel - player->getLevel());
        int32 pct = ceil((diff*100)/sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)) + baseReduction;

        return pct > 99 ? 99 : pct;
    }

    
    return baseReduction; // only base reduction
}

// SPELLS TO DISABLE IN TW
bool AzthUtils::isNotAllowedSpellForTw(SpellInfo const* spellProto) {
    uint32 lastSpell=sSpellMgr->GetLastSpellInChain(spellProto->Id);
    
    switch(lastSpell) {
        // flask of the north
        case 67016:
        case 67017:
        case 67018:
        case 67019:
            return true;
    }
    
    return false;
}

// SPELLS TO ENABLE IN TW
bool AzthUtils::isSpecialSpellForTw(SpellInfo const* spellProto) {
    uint32 lastSpell=sSpellMgr->GetLastSpellInChain(spellProto->Id);
    
    switch(lastSpell) {
        // PALADIN
        case 32223: // Crusader Aura
        case 54428: // Divine Plea
        case 53736: // Seal of Corruption
        case 31884: // Avenging Wrath
        case 642:   // Divine shield
        case 1038:  // Hand of Salvation
        case 20166: // Seal of Wisdom
        case 20165: // Seal of Light
        case 20164: // Seal of Justice
        case 20925: // Holy Shield
        case 31801: // Seal of Vengeance
        case 6940:  // Hand of Sacrifice
        case 19746: // concentration aura
        case 25858: // Greater blessing of Kings
        case 53563: // Beacon of Light
        case 20911: // Blessing of Sanctuary
        case 25899: // Greater blessing of sanctuary
            
            
        // WARRIOR
        case 2458:  // Berserker stance
        case 18499: // Berserker rage 
        case 12292: // Death Wish
        case 55694: // Enraged Regeneration
        case 23920: // Spell reflection
        case 871:   // Shield Wall
        case 1719:  // Recklessness
            
        // DRUID
        case 50334: // berserker
        case 29166: // innervate
        case 33357: // dash
        case 2893:  // abolish poison
        case 33891: // tree form
        case 24858: // moonkin form
        case 48451: // lifebloom
        case 22812: // barkskin
        case 53251: // wild Growth
        case 62071: // savage roar
            
        // DK
        case 48263: // Frost Presence
        case 48265: // Unholy Presence
        case 48266: // Blood Presence
        case 48792: // Icebound Fortitude
        case 3714:  // Path of Frost
        case 48707: // Anti-Magic Shell
        case 49016: // Hysteria
        case 49039: // Lichborne
        case 49796: // Deathchill
        case 49028: // Dancing rune weapon
            
        // HUNTER
        case 19263: // Deterrence
        case 5384:  // Feign Death
        case 13161: // Aspect of the Beast
        case 62757: // Call Stabled Pet
        case 13159: // Aspect of the Pack
        case 34026: // Kill command
        case 34027: // Kill Command (triggered)
        case 19878: // Track Demons
        case 19879: // Track Dragonkin
        case 19880: // Track Elementals
        case 19882: // Track Giants
        case 5225:  // Track Humanoids
        case 3045:  // Rapid Fire
            
        // WARLOCK
        case 48018: //  Demon circle: summon
        case 132:   //  Detect invisibility
        case 47893: // Fel armor
        case 50589: // Immolation
        case 48020: // demonic circle: teleport
            
        // SHAMAN
        case 2825: // bloodlust
        case 6495: // sentry totem
        case 546:  // water walking
        case 30823: // shamanistic rage
        case 49284: // earth shield
        case 16191: // Mana Tide (ability triggered by Mana Tide Totem)
            
        // PRIEST
        case 47585: // Dispersion
        case 1706: // levitate
        case 552:  // abolish disease
        case 528:  // cure disease
        case 47788: // guardian spirit
        case 10060: // power infusion
        case 64904: // Hymn of Hope (triggered)
            
        // ROGUE
        case 51713: // Shadow Dance
            
        // MAGE
        case 66:    // Invisibility
        case 11129: // Combustion
        case 43046: // Molten Armor
        case 43008: // Ice Armor
        case 43024: // Mage Armor
        case 45438: // Ice block
        case 43039: // Ice Barrier
        case 44645: // waterbolt (mage elemental)
            return true;
    }
    
    return false;
}

bool AzthUtils::canScaleSpell(SpellInfo const* spellProto) {
    // do not allow spell with reagents to be scaled
    for (uint32 i = 0; i < MAX_SPELL_REAGENTS; i++)
    {
        if (spellProto->Reagent[i] > 0)
            return false;
    }

    return true;
}

bool AzthUtils::disableEnchant(Player *player, SpellItemEnchantmentEntry const* pEnchant) {
    if (!player->azthPlayer->isTimeWalking(true))
        return false;
    
    if (pEnchant->GemID) {
        switch(pEnchant->GemID) {
            case 42142:
            case 36766:
            case 42148:
            case 42143:
            case 42152:
            case 42153:
            case 42146:
            case 42158:
            case 42154:
            case 42150:
            case 42156:
            case 42144:
            case 42149:
            case 36767:
            case 42145:
            case 42155:
            case 42151:
            case 42157:
                return true;
        }
    }
    
    if (pEnchant->requiredSkillValue) {
        uint32 level = player->getLevel();
        
        if (pEnchant->requiredSkillValue >= 350 && level < 65) {
            return true;
        } else if (pEnchant->requiredSkillValue >= 275 && level < 50) {
            return true;
        } else if (pEnchant->requiredSkillValue >= 200 && level < 35) {
            return true;
        } else if (pEnchant->requiredSkillValue >= 125 && level < 20) {
            return true;
        } else if (pEnchant->requiredSkillValue >= 50 && level < 10) {
            return true;
        }
    }
        
        
    return false;
}