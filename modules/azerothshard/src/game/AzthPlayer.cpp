#include "AzthPlayer.h"
#include "Chat.h"
#include "Define.h"
#include "ObjectAccessor.h"
#include "World.h"
#include "AzthLevelStat.h"
#include "Unit.h"
#include "Pet.h"
#include "Player.h"
#include "AccountMgr.h"


AzthPlayer::AzthPlayer(Player *origin) {
  playerQuestRate = sWorld->getRate(RATE_XP_QUEST);
  player = origin;
  customLang = AZTH_LOC_IT;
  std::fill_n(arena1v1Info, 7, 0);
  std::fill_n(arena3v3Info, 7, 0);
  
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
        if (!target->HasSpell(*spell))
            target->learnSpell(*spell);
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
