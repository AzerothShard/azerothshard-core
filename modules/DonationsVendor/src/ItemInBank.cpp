#include "ItemInBank.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"


ItemInBank::ItemInBank()
{
    characterGUID = uint32(0);
    itemGUID = uint32(0);
    itemEntry = uint32(0);
}


ItemInBank::ItemInBank(uint32 characterGUID, uint32 itemGUID, uint32 itemEntry)
{
	this->characterGUID = characterGUID;
    this->itemGUID = itemGUID;
    this->itemEntry = itemEntry;
}

uint32 ItemInBank::GetCharacterGUID() const
{
	return characterGUID;
}

uint32 ItemInBank::GetItemGUID() const
{
	return itemGUID;
}

uint32 ItemInBank::GetItemEntry() const
{
    return itemEntry;
}

void ItemInBank::SetBankItemsList(vector<ItemInBank> itemsInBankList)
{
    this->itemsInBankList = itemsInBankList;
}

vector<ItemInBank> ItemInBank::GetBankItemsList() const
{
    return itemsInBankList;
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