#include "AzthUtils.h"
#include "AzthLevelStat.h"
#include "Pet.h"
#include "SpellMgr.h"
#include "SpellInfo.h"



AzthUtils::AzthUtils()
{
    for (ObjectMgr::CharacterConversionMap::iterator i = sObjectMgr->FactionChangeItems.begin(); i != sObjectMgr->FactionChangeItems.end(); ++i)
        this->FactionChangeItemsHorde[i->second] = i->first;
}

AzthUtils::~AzthUtils()
{
}

void AzthUtils::loadClassSpells() {
    QueryResult res = WorldDatabase.PQuery("SELECT racemask, classmask, Spell FROM playercreateinfo_spell_custom;");

    if (!res)
    {
        return;
    }

    
    do
    {
        Field* fields = res->Fetch();
        uint32 raceMask = fields[0].GetUInt32();
        uint32 classMask = fields[1].GetUInt32();
        uint32 spellId = fields[2].GetUInt32();

        if (raceMask != 0 && !(raceMask & RACEMASK_ALL_PLAYABLE))
        {
            sLog->outErrorDb("Wrong race mask %u in `playercreateinfo_spell_custom` table, ignoring.", raceMask);
            continue;
        }

        if (classMask != 0 && !(classMask & CLASSMASK_ALL_PLAYABLE))
        {
            sLog->outErrorDb("Wrong class mask %u in `playercreateinfo_spell_custom` table, ignoring.", classMask);
            continue;
        }
        
        
        for (uint32 raceIndex = RACE_HUMAN; raceIndex < MAX_RACES; ++raceIndex)
        {
            if (raceMask == 0 || ((1 << (raceIndex - 1)) & raceMask))
            {
                for (uint32 classIndex = CLASS_WARRIOR; classIndex < MAX_CLASSES; ++classIndex)
                {
                    if (classMask == 0 || ((1 << (classIndex - 1)) & classMask))
                    {
                        startSpells[raceIndex][classIndex].push_back(spellId);
                    }
                }
            }
        }
    } while (res->NextRow());
}

void AzthUtils::learnClassSpells(Player* player, bool /*new_level*/)
{       
    ChrClassesEntry const* classEntry = sChrClassesStore.LookupEntry(player->getClass());
    if (!classEntry)
        return;

    std::list<uint32> spells=startSpells[player->getRace(true)][player->getClass()];

    for (std::list<uint32>::iterator it = spells.begin(); it != spells.end(); ++it)
    {
        uint32 s = *it;
        if (!s)
            continue;

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(s);
        
        do {
        
            if (!spellInfo)
                continue;
            
            // skip wrong class/race skills
            //if (!player->IsSpellFitByClassAndRace(spellInfo->Id))
            //    continue;

            // skip other spell families
            //if (spellInfo->SpellFamilyName != family)
            //    continue;

            // skip spells with first rank learned as talent (and all talents then also)
            uint32 firstRank = sSpellMgr->GetFirstSpellInChain(spellInfo->Id);
            if (GetTalentSpellCost(firstRank) > 0)
                continue;

            // skip broken spells
            if (!SpellMgr::IsSpellValid(spellInfo))
                continue;

            player->learnSpell(spellInfo->Id);

        } while((spellInfo = spellInfo->GetPrevRankSpell())); // learn prev ranks
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



std::vector<std::string> getCategoryIconAndNameByItemType(uint32 itemType)
{
    std::string INVTYPE_NON_EQUIP_STRING = "Generic";
    std::string INVTYPE_HEAD_STRING = "Heads";
    std::string INVTYPE_NECK_STRING = "Necks";
    std::string INVTYPE_SHOULDERS_STRING = "Shoulders";
    std::string INVTYPE_BODY_STRING = "Body";
    std::string INVTYPE_CHEST_STRING = "Chests";
    std::string INVTYPE_WAIST_STRING = "Waists";
    std::string INVTYPE_LEGS_STRING = "Legs";
    std::string INVTYPE_FEET_STRING = "Feets";
    std::string INVTYPE_WRISTS_STRING = "Wrists";
    std::string INVTYPE_HANDS_STRING = "Hands";
    std::string INVTYPE_FINGER_STRING = "Fingers";
    std::string INVTYPE_TRINKET_STRING = "Trinkets";
    std::string INVTYPE_WEAPON_STRING = "Weapons";
    std::string INVTYPE_SHIELD_STRING = "Shields";
    std::string INVTYPE_RANGED_STRING = "Ranged";
    std::string INVTYPE_CLOAK_STRING = "Cloaks";
    std::string INVTYPE_2HWEAPON_STRING = "2H Weapons";
    std::string INVTYPE_BAG_STRING = "Bags";
    std::string INVTYPE_TABARD_STRING = "Tabards";
    std::string INVTYPE_ROBE_STRING = "Robes";
    std::string INVTYPE_WEAPONMAINHAND_STRING = "Main Hand Weapons";
    std::string INVTYPE_WEAPONOFFHAND_STRING = "Off Hand Weapons";
    std::string INVTYPE_HOLDABLE_STRING = "Holdables";
    std::string INVTYPE_AMMO_STRING = "Ammo";
    std::string INVTYPE_THROWN_STRING = "Thrown";
    std::string INVTYPE_RANGEDRIGHT_STRING = "Right Ranged";
    std::string INVTYPE_QUIVER_STRING = "Quiver";
    std::string INVTYPE_RELIC_STRING = "Relics";
    
    std::vector<std::string> category;

    switch (itemType)
    {
    case INVTYPE_NON_EQUIP:
        category.push_back(INVTYPE_NON_EQUIP_STRING);
        category.push_back("INV_Misc_Questionmark");
        break;
    case INVTYPE_HEAD:
        category.push_back(INVTYPE_HEAD_STRING);
        category.push_back("INV_Helmet_12");
        break;
    case INVTYPE_NECK:
        category.push_back(INVTYPE_NECK_STRING);
        category.push_back("INV_Jewelry_Necklace_02");
        break;
    case INVTYPE_SHOULDERS:
        category.push_back(INVTYPE_SHOULDERS_STRING);
        category.push_back("INV_Shoulder_92");
        break;
    case INVTYPE_BODY:
        category.push_back(INVTYPE_BODY_STRING);
        category.push_back("INV_Shirt_04");
        break;
    case INVTYPE_CHEST:
        category.push_back(INVTYPE_CHEST_STRING);
        category.push_back("INV_Chest_Leather_05");
        break;
    case INVTYPE_WAIST:
        category.push_back(INVTYPE_WAIST_STRING);
        category.push_back("INV_Belt_45b");
        break;
    case INVTYPE_LEGS:
        category.push_back(INVTYPE_LEGS_STRING);
        category.push_back("INV_Pants_Cloth_10");
        break;
    case INVTYPE_FEET:
        category.push_back(INVTYPE_FEET_STRING);
        category.push_back("INV_Boots_02");
        break;
    case INVTYPE_WRISTS:
        category.push_back(INVTYPE_WRISTS_STRING);
        category.push_back("INV_Bracer_21c");
        break;
    case INVTYPE_HANDS:
        category.push_back(INVTYPE_HANDS_STRING);
        category.push_back("INV_Gauntlets_12");
        break;
    case INVTYPE_FINGER:
        category.push_back(INVTYPE_FINGER_STRING);
        category.push_back("INV_Jewelry_Talisman_08");
        break;
    case INVTYPE_TRINKET:
        category.push_back(INVTYPE_TRINKET_STRING);
        category.push_back("INV_Jewelry_Trinket_04");
        break;
    case INVTYPE_WEAPON:
        category.push_back(INVTYPE_WEAPON_STRING);
        category.push_back("INV_Weapon_Glave_01");
        break;
    case INVTYPE_SHIELD:
        category.push_back(INVTYPE_SHIELD_STRING);
        category.push_back("INV_Shield_05");
        break;
    case INVTYPE_RANGED:
        category.push_back(INVTYPE_RANGED_STRING);
        category.push_back("INV_Weapon_Crossbow_23");
        break;
    case INVTYPE_CLOAK:
        category.push_back(INVTYPE_CLOAK_STRING);
        category.push_back("INV_Misc_Cape_18");
        break;
    case INVTYPE_2HWEAPON:
        category.push_back(INVTYPE_2HWEAPON_STRING);
        category.push_back("INV_Mace_72");
        break;
    case INVTYPE_BAG:
        category.push_back(INVTYPE_BAG_STRING);
        category.push_back("INV_Misc_Bag_09");
        break;
    case INVTYPE_TABARD:
        category.push_back(INVTYPE_TABARD_STRING);
        category.push_back("INV_Shirt_Guildtabard_01");
        break;
    case INVTYPE_ROBE:
        category.push_back(INVTYPE_ROBE_STRING);
        category.push_back("INV_Robe_Emberweavegown_a_01");
        break;
    case INVTYPE_WEAPONMAINHAND:
        category.push_back(INVTYPE_WEAPONMAINHAND_STRING);
        category.push_back("INV_Mace_48");
        break;
    case INVTYPE_WEAPONOFFHAND:
        category.push_back(INVTYPE_WEAPONOFFHAND_STRING);
        category.push_back("INV_Offhand_Dalaran_d_01");
        break;
    case INVTYPE_HOLDABLE:
        category.push_back(INVTYPE_HOLDABLE_STRING);
        category.push_back("INV_Misc_Giftwrap_02");
        break;
    case INVTYPE_AMMO:
        category.push_back(INVTYPE_AMMO_STRING);
        category.push_back("INV_Misc_Ammo_Arrow_03");
        break;
    case INVTYPE_THROWN:
        category.push_back(INVTYPE_THROWN_STRING);
        category.push_back("INV_Throwingaxe_03");
        break;
    case INVTYPE_RANGEDRIGHT:
        category.push_back(INVTYPE_RANGEDRIGHT_STRING);
        category.push_back("INV_Weapon_Rifle_27");
        break;
    case INVTYPE_QUIVER:
        category.push_back(INVTYPE_QUIVER_STRING);
        category.push_back("INV_Misc_Quiver");
        break;
    case INVTYPE_RELIC:
        category.push_back(INVTYPE_RELIC_STRING);
        category.push_back("INV_Relics_Libramofgrace");
        break;
    }

    return category;
}

std::string GetItemIcon(uint32 entry, uint32 width, uint32 height, int x, int y)
{
    std::ostringstream ss;
    ss << "|TInterface";
    const ItemTemplate* temp = sObjectMgr->GetItemTemplate(entry);
    const ItemDisplayInfoEntry* dispInfo = NULL;
    if (temp)
    {
        dispInfo = sItemDisplayInfoStore.LookupEntry(temp->DisplayInfoID);
        if (dispInfo)
            ss << "/ICONS/" << dispInfo->inventoryIcon;
    }
    if (!dispInfo)
        ss << "/InventoryItems/WoWUnknownItem01";
    ss << ":" << width << ":" << height << ":" << x << ":" << y << "|t";
    return ss.str();
}