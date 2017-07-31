#include "AzthUtils.h"
#include "AzthLevelStat.h"
#include "Pet.h"
#include "SpellMgr.h"
#include "SpellInfo.h"



AzthUtils::AzthUtils()
{
}

AzthUtils::~AzthUtils()
{
}

void AzthUtils::learnClassSpells(Player* player, bool new_level)
{
     uint32 temp[] = {
        64380, 23885, 23880, 44461, 25346, 10274, 10273, 8418,
        8419, 7270, 7269, 7268, 54648, 12536, 24530, 70909,
        12494, 57933, 24224, 27095, 27096, 27097, 27099, 32841,
        56131, 56160, 56161, 48153, 34754, 64844, 64904, 48085,
        33110, 48084, 28276, 27874,     27873, 7001, 49821, 53022,
        47757, 47750, 47758, 47666, 53001, 52983, 52998, 52986,
        52987, 52999, 52984, 53002, 53003, 53000, 52988, 52985,
        42208, 42209, 42210, 42211, 42212, 42213, 42198, 42937,
        42938, 12484, 12485, 12486, 44461, 55361, 55362, 34913,
        43043, 43044, 38703, 38700, 27076, 42844, 42845, 64891,
        25912, 25914, 25911, 25913, 25902, 25903, 27175, 27176,
        33073, 33074, 48822, 48820, 48823, 48821, 20154, 25997,
        20467, 20425, 67, 26017, 34471, 53254, 13812, 14314,
        14315, 27026, 49064, 49065, 60202, 60210, 13797, 14298,
        14299, 14300, 14301, 27024, 49053, 49054, 52399, 1742,
        24453, 53548, 53562, 52016, 26064, 35346, 57386, 57389,
        57390, 57391, 57392, 57393, 55509, 35886, 43339, 45297,
        45298, 45299, 45300, 45301, 45302, 49268, 49269, 8349,
        8502, 8503, 11306, 11307, 25535, 25537, 61650, 61654,
        63685, 45284, 45286, 45287, 45288, 45289, 45290, 45291,
        45292, 45293, 45294, 45295, 45296, 49239, 49240, 26364,
        26365, 26366, 26367, 26369, 26370, 26363, 26371, 26372,
        49278, 49279, 32176, 32175, 21169, 47206, 27285, 47833,
        47836, 42223, 42224, 42225, 42226, 42218, 47817, 47818,
        42231, 42232, 42233, 42230, 48466, 44203, 44205, 44206,
        44207, 44208, 48444, 48445, 33891, 52374, 57532, 59921,
        52372, 49142, 52375, 47633, 47632, 52373, 50536, 27214,
        47822, 11682, 11681, 5857, 1010, 24907, 24905, 53227,
        61391, 61390, 61388, 61387, 64801, 5421, 9635, 1178,
        20186, 20185, 20184, 20187, 25899, 24406, 50581, 30708
                                };

    std::vector<uint32> ignoreSpells = std::vector<uint32> (temp, temp + sizeof(temp)/sizeof(temp[0]));

        
    ChrClassesEntry const* classEntry = sChrClassesStore.LookupEntry(player->getClass());
    if (!classEntry)
        return;
    uint32 family = classEntry->spellfamily;

    for (uint32 i = 0; i < sSkillLineAbilityStore.GetNumRows(); ++i)
    {
        SkillLineAbilityEntry const* entry = sSkillLineAbilityStore.LookupEntry(i);
        if (!entry)
            continue;

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(entry->spellId);
        if (!spellInfo)
            continue;

        // skip server-side/triggered spells
        if (spellInfo->SpellLevel == 0)
            continue;

        bool ignored = false;
        for (std::vector<uint32>::const_iterator itr = ignoreSpells.begin(); itr != ignoreSpells.end(); ++itr)
        if (spellInfo->Id == (*itr)) {
            ignored = true;
            break;
        }
        
        if (ignored)
            continue;
        
        // skip wrong class/race skills
        if (!player->IsSpellFitByClassAndRace(spellInfo->Id))
            continue;

        // skip other spell families
        if (spellInfo->SpellFamilyName != family)
            continue;

        // skip spells with first rank learned as talent (and all talents then also)
        uint32 firstRank = sSpellMgr->GetFirstSpellInChain(spellInfo->Id);
        if (GetTalentSpellCost(firstRank) > 0)
            continue;

        // skip broken spells
        if (!SpellMgr::IsSpellValid(spellInfo))
            continue;

        player->learnSpell(spellInfo->Id);
    }
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
            uint32 req = ((pProto->ItemLevel - 92) / 2) + 61;
            return req > 70 ? 70 : req;
        }
            
        // tbc epic
        if (pProto->ItemLevel > 115 && pProto->ItemLevel <= 164 && pProto->Quality >= ITEM_QUALITY_EPIC) {
            uint32 req = ((pProto->ItemLevel - 115) / 4) + 61;
            return req > 70 ? 70 : req;
        }
        
        //if (pProto->ItemLevel > 130 && pProto->ItemLevel <= 284)
        //    return 80;
        
        // 130 - 284
        return sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
    }

    if (pProto->RequiredLevel == 0)
        return 1;

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
