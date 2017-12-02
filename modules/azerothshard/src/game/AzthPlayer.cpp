#include "AzthPlayer.h"
#include "Chat.h"
#include "Define.h"
#include "ObjectAccessor.h"
#include "World.h"
#include "AzthLevelStat.h"
#include "Unit.h"
#include "Pet.h"
#include "Player.h"
#include "SpellAuras.h"
#include "AccountMgr.h"

#include "MapManager.h"
#include "MapInstanced.h"
#include "InstanceScript.h"
#include "ArenaSeason.h"
#include "AzthAchievement.h"
#include "AzthUtils.h"
#include "LFGMgr.h"

class Aura;

AzthPlayer::AzthPlayer(Player *origin) {
  playerQuestRate = sWorld->getRate(RATE_XP_QUEST);
  player = origin;
  customLang = AZTH_LOC_IT;
  std::fill_n(arena1v1Info, 7, 0);
  std::fill_n(arena3v3Info, 7, 0);
  autoScalingPending = 0;
  
  m_itemsInBank.clear();
}

AzthPlayer::~AzthPlayer() {
    m_itemsInBank.clear();
}

void AzthPlayer::setCustLang(AzthCustomLangs code) {
    this->customLang = code;
}

AzthCustomLangs AzthPlayer::getCustLang()
{
    return this->customLang;
}

bool AzthPlayer::AzthMaxPlayerSkill() {

    Player* target = player;
    if (!target) {
        return false;
    }
    if (target->getLevel() < 80) {
        ChatHandler(player->GetSession()).PSendSysMessage(LANG_LEVEL_MINREQUIRED, 80);
        return false;
    }

    static const SkillSpells spells[] = { ONE_HAND_AXES, TWO_HAND_AXES, ONE_HAND_MACES,
        TWO_HAND_MACES, POLEARMS, ONE_HAND_SWORDS, TWO_HAND_SWORDS, STAVES, BOWS,
        GUNS, DAGGERS, WANDS, CROSSBOWS, FIST_WEAPONS };

    std::list<SkillSpells> learnList;
    std::size_t max=(sizeof(spells)/sizeof(spells[0]));
    for (std::size_t s = 0; s<max; s++) {
        SkillSpells spell = spells[s];
        switch (target->getClass()) {
        case CLASS_WARRIOR:
            if (spell != WANDS)
                learnList.push_back(spell);
            break;
        case CLASS_DEATH_KNIGHT:
        case CLASS_PALADIN:
            if (spell != STAVES && spell != BOWS && spell != GUNS && spell != DAGGERS &&
                spell != WANDS && spell != CROSSBOWS && spell != FIST_WEAPONS)
                learnList.push_back(spell);
            break;
        case CLASS_HUNTER:
            if (spell != ONE_HAND_MACES && spell != TWO_HAND_MACES && spell != WANDS)
                learnList.push_back(spell);
            break;
        case CLASS_ROGUE:
            if (spell != TWO_HAND_AXES && spell != TWO_HAND_MACES && spell != POLEARMS &&
                spell != TWO_HAND_SWORDS && spell != STAVES && spell != WANDS)
                learnList.push_back(spell);
            break;
        case CLASS_PRIEST:
            if (spell == WANDS || spell == ONE_HAND_MACES || spell == STAVES ||
                spell == DAGGERS)
                learnList.push_back(spell);
            break;
        case CLASS_SHAMAN:
            if (spell != ONE_HAND_SWORDS && spell != TWO_HAND_SWORDS && spell != POLEARMS &&
                spell != BOWS && spell != GUNS && spell != WANDS && spell != CROSSBOWS)
                learnList.push_back(spell);
            break;
        case CLASS_WARLOCK:
        case CLASS_MAGE:
            if (spell == WANDS || spell == ONE_HAND_SWORDS || spell == STAVES ||
                spell == DAGGERS)
                learnList.push_back(spell);
            break;
        case CLASS_DRUID:
            if (spell != ONE_HAND_SWORDS && spell != TWO_HAND_SWORDS &&
                spell != BOWS && spell != GUNS && spell != WANDS && spell != CROSSBOWS &&
                spell != ONE_HAND_AXES && spell != TWO_HAND_AXES)
                learnList.push_back(spell);
            break;
        default:
            break;
        }
    }

    for (std::list<SkillSpells>::const_iterator spell = learnList.begin(), end = learnList.end(); spell != end; ++spell) {
        if (!target->HasSpell(uint32(*spell)))
            target->learnSpell(uint32(*spell));
    }

    target->UpdateSkillsToMaxSkillsForLevel();
    return true;
}

void AzthPlayer::AddBankItem(uint32 itemEntry, uint32 itemGuid)
{
    m_itemsInBank[itemEntry] = itemGuid;
}

void AzthPlayer::DelBankItem(uint32 itemEntry) {
    m_itemsInBank.erase(itemEntry);
}

AzthPlayer::ItemInBankMap & AzthPlayer::GetBankItemsList()
{
    return m_itemsInBank;
}

bool AzthPlayer::canEnterMap(MapEntry const* entry, InstanceTemplate const* /*instance*/, bool /*loginCheck*/) {
        
    // CHECK BG or ARENA requirements (item level limit for example)
    if (entry->IsBattlegroundOrArena() && !canJoinQueue(AZTH_QUEUE_BG_OR_ARENA)) {
        return false;
    }
    
    uint32 curDimension = getCurrentDimensionByAura();
    
    Difficulty targetDifficulty;
    targetDifficulty = player->GetDifficulty(entry->IsRaid());
    
    if (!entry->IsBattlegroundOrArena() && !sLFGMgr->inLfgDungeonMap(player->GetGUID(), entry->MapID, targetDifficulty)) {
        
        // we could use GetContentLevelsForMapAndZone(uint32 mapid, uint32 zoneId) for a more accurate chech (?)

        if (curDimension == DIMENSION_60 && entry->Expansion() > 0) {
            // CLASSIC EXPANSION CHECK
            ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000 Sei nella dimensione Classic, non è possibile accedere a mappe di espansioni più recenti|r");
            player->SendTransferAborted(entry->MapID, TRANSFER_ABORT_MAP_NOT_ALLOWED);
            return false;
        } if (curDimension == DIMENSION_70 && entry->Expansion() > 1) {
            // TBC EXPANSION CHECK
            ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000 Sei nella dimensione TBC, non è possibile accedere a mappe di espansioni più recenti|r");
            player->SendTransferAborted(entry->MapID, TRANSFER_ABORT_MAP_NOT_ALLOWED);
            return false;
        }
    }

    /**
     *  FULL PVP CHECK
     */
    if (isPvP() && (entry->IsRaid() || entry->IsDungeon())) {
        ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000 I personaggi Full PvP non hanno accesso ai Dungeons e Raids|r");
        player->SendTransferAborted(entry->MapID, TRANSFER_ABORT_MAP_NOT_ALLOWED);
        return false;
    }
    
    return true;
}

bool AzthPlayer::canGroup(Player* with)
{
    if (with) {
        uint32 curDimPlayer=player->azthPlayer->getCurrentDimensionByPhase();
        uint32 curDimWith=with->azthPlayer->getCurrentDimensionByPhase();
        /*if (curDimPlayer != curDimWith) {
            ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000 Non è possibile entrare in gruppo con PG di dimensioni diverse|r");
            return false;
        }*/

        if (curDimPlayer == DIMENSION_GUILD || curDimWith == DIMENSION_GUILD) {
            if (player->GetGuildId() != with->GetGuildId()) {
                ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000 Non è possibile entrare in gruppo con PG di altre gilde presenti nella dimensione Guild World|r");
                ChatHandler(with->GetSession()).PSendSysMessage("|cffff0000 Non è possibile entrare in gruppo con PG di altre gilde presenti nella dimensione Guild World|r");
                return false;
            }
        }
    }

    return true;
}


bool AzthPlayer::canJoinQueue(AzthQueueMasks type) {  
    if ((type & AZTH_QUEUE_ARENA) == type) {
        if (!sASeasonMgr->canJoinArenaOrBg(player))
            return false;
    }
    
    return true;
}

// this is the most accurate check to avoid exploit in some conditions
uint32 AzthPlayer::getCurrentDimensionByPhase() const {
    return sAzthUtils->getCurrentDimensionByPhase(player->GetPhaseMask());
}

uint32 AzthPlayer::getCurrentDimensionByMark() const {
    Aura *aura = player->GetAura(DIMENSION_SPELL_MARK);

    // if no aura, then assume we're on DIMENSION_NORMAL
    uint32 currentDim = aura ? aura->GetStackAmount() : DIMENSION_NORMAL;
    
    return currentDim;
}

uint32 AzthPlayer::getCurrentDimensionByAura() const {
    uint32 dim = getCurrentDimensionByPhase();
    
    if (dim == DIMENSION_NORMAL)
        dim = getCurrentDimensionByMark(); // double integrity check
        
    return player->HasAura(DIMENSION_SPELL_MARK+dim) ? dim : DIMENSION_NORMAL;
}

bool AzthPlayer::hasDimSpecificAura(uint32 dim) {
    return player->HasAura(DIMENSION_SPELL_MARK+dim);
}

bool AzthPlayer::canExplore() {
    uint32 currentDim =  getCurrentDimensionByPhase();
    
    if (sAzthUtils->isPhasedDimension(currentDim))
        return false;
    
    return true;
}

bool AzthPlayer::changeDimension(uint32 dim, bool validate /* = false*/, bool temp /* =false */) {
    // dimension we're marked for
    // this property will contain the special dimension 
    // even if we're temporary in normal (no phasing auras)
    uint32 markedDimension =  getCurrentDimensionByMark();
    // real dimension based on auras. If we've the
    // DIMENSION_SPELL_MARK but no auras then this property will contain
    // DIM_NORMAL (temporary case)
    uint32 auraDimension = getCurrentDimensionByAura();
    // this is the most accurate check for validity since
    // with an exploit we could have a special dimension phase even without auras
    // then we must not allow it
    //uint32 phaseDimension     = getCurrentDimensionByPhase();
    bool changed=false;
    
    /*bool inGmParty=false;
    if (player->GetGroup()) {
        uint32 leaderGuid=player->GetGroup()->GetLeaderGUID();
        if (Player* leader = ObjectAccessor::FindPlayer(leaderGuid)) {
            if (leader->GetSession()->GetSecurity() > SEC_PLAYER) {
                inGmParty=true;
            }
        }
    }*/
    
    if (validate) {
        /*if (auraDimension != dim && !inGmParty && player->GetGroup()) {
            ChatHandler(player->GetSession()).SendSysMessage("Non è possibile cambiare dimensione mentre si è in gruppo");
            return false;
        }*/
        
        if (dim == DIMENSION_60) {
            if (player->getLevel() > 60 && player->azthPlayer->GetTimeWalkingLevel() != TIMEWALKING_LVL_AUTO) {
                ChatHandler(player->GetSession()).PSendSysMessage("E' necessario essere di livello 60 o inferiore per entrare in questa dimensione, prova ad usare il Timewalking");
                return false;
            }
        }
        
        if (dim == DIMENSION_70) {
            if (player->getLevel() > 70 && player->azthPlayer->GetTimeWalkingLevel() != TIMEWALKING_LVL_AUTO) {
                ChatHandler(player->GetSession()).PSendSysMessage("E' necessario essere di livello 70 o inferiore per entrare in questa dimensione, prova ad usare il Timewalking");
                return false;
            }
        }
        
        if (dim == DIMENSION_GUILD) {
            if (!player->GetGuild()) {
                ChatHandler(player->GetSession()).PSendSysMessage("E' necessario essere in una gilda per entrare in questa dimensione!");
                return false;
            }
        }
        
        if (dim == DIMENSION_GM) {
            if (player->GetSession()->GetSecurity() <= SEC_PLAYER) {
                ChatHandler(player->GetSession()).PSendSysMessage("E' necessario essere un Game Master per accedere a questa dimensione!");
                return false;
            }
        }
    }
    
    /*if (!inGmParty) {
        if (phaseDimension != dim && player->GetGroup()) {
            // if we changed phase and we're grouped
            // then we should ungroup to avoid exploits
            ///- If the player is in a group (or invited), remove him. If the group if then only 1 person, disband the group.
            player->UninviteFromGroup();

            if (player->GetGroup())
                player->RemoveFromGroup();
        }
    }*/
    
    if (!temp) {
        if (markedDimension > DIMENSION_NORMAL) {
            player->RemoveAura(DIMENSION_SPELL_MARK);
            changed=true;
        }
    }
    
    if (auraDimension > DIMENSION_NORMAL) {
        player->RemoveAura(DIMENSION_SPELL_MARK + auraDimension);
        changed=true;
    }

    if (dim > DIMENSION_NORMAL) {
        if (!temp) {
            player->SetAuraStack(DIMENSION_SPELL_MARK, player, dim);
        }

        player->SetAuraStack(DIMENSION_SPELL_MARK + dim, player, 1);
        changed=true;
    }
    
    if (changed) {
        player->CastSpell(player, 35517, true);
        std::string msg="|cffff0000 Sei stato trasportato nella dimensione ";
        std::string dimName = sAzthUtils->getDimensionName(dim);
        std::string suffix  = temp ? "temporaneamente" : "";
        msg += " <"+dimName+"> "+suffix+" |r";

        ChatHandler(player->GetSession()).PSendSysMessage("%s", msg.c_str());
    }

    return changed;
}

bool AzthPlayer::canCompleteCriteria(AchievementCriteriaEntry const* criteria) {
    uint32 currentDimension = getCurrentDimensionByAura();

    if (sAzthAchievementMgr->achievementList.find(criteria->ID) == sAzthAchievementMgr->achievementList.end()) {
        if (sAzthUtils->isPhasedDimension(currentDimension))
            return false;

        return true;
    }

    AzthAchievement achi = sAzthAchievementMgr->achievementList[criteria->ID];
    
    /**
    *  CHECK REQUIREMENTS
    */
    if (achi.GetReqDimension() > 0) {
        if (achi.GetReqDimension() != currentDimension)
            return false;
    } else if (sAzthUtils->isPhasedDimension(currentDimension))
        return false;

    uint32 groupLevel= player->azthPlayer->getGroupLevel();
    uint32 specialGroupLevel = player->azthPlayer->getGroupLevel(false);
    uint32 level = groupLevel > 0 ? groupLevel : player->getLevel();
    uint32 specialLevel = specialGroupLevel > 0 ? specialGroupLevel : player->azthPlayer->GetTimeWalkingLevel();
    
    // skip rewards if you don't have the required special level
    if (achi.GetSpecialLevelReq() > 0 && 
        (!player->azthPlayer->isTimeWalking() || specialLevel != achi.GetSpecialLevelReq()) ) {
        return false;
    }
    
    if (level < achi.GetLevelMin() || level  > achi.GetLevelMax() ) {
        return false;
    }
    /*
    *  END 
    */

    return true;
}

uint32 AzthPlayer::getMaxItemLevelByStatus() {
    if (isTimeWalking()) {
        uint32 twLvl=GetTimeWalkingLevel();
        
        uint32 twLimit = getTwItemLevel(twLvl);
        
        if (twLimit)
            return twLimit;
    }
    
    return 0;
}

bool AzthPlayer::canEquipItem(ItemTemplate const* proto) {
    // it is needed to avoid equip in empty slots
    if (hasGear())
        return false;

    if (!sASeasonMgr->checkItem(proto, player))
        return false;
    
    if (!checkItem(proto))
        return false;
    
    //Double ashen block system
    if(sAzthUtils->isAshenBand(proto->ItemId)) {
        // finger slots
        for (uint32 INVENTORY_INDEX = EQUIPMENT_SLOT_FINGER1; INVENTORY_INDEX <= EQUIPMENT_SLOT_FINGER2; INVENTORY_INDEX++)
        {            
            Item* itemToCheck = player->GetItemByPos(INVENTORY_SLOT_BAG_0, INVENTORY_INDEX);
            if (itemToCheck != nullptr)
            {
                if (sAzthUtils->isAshenBand(itemToCheck->GetEntry())) {
                    ChatHandler(player->GetSession()).PSendSysMessage("Are you a double ashen abuser?!");
                    return false;
                }
            }
        }
    }
    
    return true;
}

//
// Passing player argument will check the player state and automatically shows a message
// 
bool AzthPlayer::checkItem(ItemTemplate const* proto) {
    uint32 _ilvl=getMaxItemLevelByStatus();
    if (_ilvl && !sAzthUtils->checkItemLvL(proto,_ilvl)) {
        ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000|Hitem:%u::::::::::::|h[%s]|h|r ha un livello troppo alto!", proto->ItemId, proto->Name1.c_str());
        return false;
    }
    
    return true;
}

bool AzthPlayer::checkItems(uint32 iLvlMax, uint8 type /*=0*/) {   
    if (player->IsGameMaster())
    {
        return true;
    }
    
    if (type==0) {
        // cap limit for each item
        uint32 INVENTORY_END = 18;
        uint32 counter = 0;

        for (uint32 INVENTORY_INDEX = 0; INVENTORY_INDEX <= INVENTORY_END; INVENTORY_INDEX++)
        {
            // don't check tabard, ranged, offhand or shirt
            if (INVENTORY_INDEX == EQUIPMENT_SLOT_TABARD || INVENTORY_INDEX == EQUIPMENT_SLOT_RANGED || INVENTORY_INDEX == EQUIPMENT_SLOT_OFFHAND || INVENTORY_INDEX == EQUIPMENT_SLOT_BODY)
                continue;
            
            Item* itemToCheck = player->GetItemByPos(INVENTORY_SLOT_BAG_0, INVENTORY_INDEX);
            if (itemToCheck != nullptr)
            {
                if (!sAzthUtils->checkItemLvL(itemToCheck->GetTemplate(), iLvlMax))
                {
                ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000|Hitem:%u::::::::::::|h[%s]|h|r ha un livello troppo alto!", itemToCheck->GetTemplate()->ItemId, itemToCheck->GetTemplate()->Name1.c_str());
                counter++;
                }
            }
        }
        
        return counter == 0;
    } else {
        uint32 avg=0;
        switch(type) {
            case 1:
                avg=player->GetAverageItemLevel();
            break;
            case 2:
                avg=player->GetAverageItemLevelForDF();
            break;
        }
        
        if (avg > iLvlMax) {
            ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000 Il tuo item level medio è troppo alto: %u|r",avg);
            return false;
        } else {
            return true;
        }
    }
}

bool AzthPlayer::isPvPFlagOn(bool def) {
    if (getCurrentDimensionByPhase() == DIMENSION_PVP) {
        player->SetByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_PVP);
        return true;
    }

    return def;
}


bool AzthPlayer::isFFAPvPFlagOn(bool def) {
    if (getCurrentDimensionByPhase() == DIMENSION_PVP) {
        player->SetByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP);
        return true;
    }

    return def;
}
