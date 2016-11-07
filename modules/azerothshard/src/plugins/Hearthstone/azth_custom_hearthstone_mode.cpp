// Copyright (c) 2016 AzerothCore
// Author: Gargarensis
// Refactored by: mik1893 
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.

#include "azth_custom_hearthstone_mode.h"

void HearthstoneMode::AzthSendListInventory(uint64 vendorGuid, WorldSession * session, uint32 extendedCostStartValue)
{
    ;//sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Sent SMSG_LIST_INVENTORY");

    Creature* vendor = session->GetPlayer()->GetNPCIfCanInteractWith(vendorGuid, UNIT_NPC_FLAG_VENDOR);
    if (!vendor)
    {
        ;//sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: SendListInventory - Unit (GUID: %u) not found or you can not interact with him.", uint32(GUID_LOPART(vendorGuid)));
        session->GetPlayer()->SendSellError(SELL_ERR_CANT_FIND_VENDOR, NULL, 0, 0);
        return;
    }

    // remove fake death
    if (session->GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        session->GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    // Stop the npc if moving
    if (vendor->HasUnitState(UNIT_STATE_MOVING))
        vendor->StopMoving();

    VendorItemData const* items = vendor->GetVendorItems();
    if (!items)
    {
        WorldPacket data(SMSG_LIST_INVENTORY, 8 + 1 + 1);
        data << uint64(vendorGuid);
        data << uint8(0);                                   // count == 0, next will be error code
        data << uint8(0);                                   // "Vendor has no inventory"
        session->SendPacket(&data);
        return;
    }

    uint8 itemCount = items->GetItemCount();
    uint8 count = 0;

    WorldPacket data(SMSG_LIST_INVENTORY, 8 + 1 + itemCount * 8 * 4);
    data << uint64(vendorGuid);

    size_t countPos = data.wpos();
    data << uint8(count);

    float discountMod = session->GetPlayer()->GetReputationPriceDiscount(vendor);

    for (uint8 slot = 0; slot < itemCount; ++slot)
    {
        if (VendorItem const* item = items->GetItem(slot))
        {
            if (ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(item->item))
            {
                if (!(itemTemplate->AllowableClass & session->GetPlayer()->getClassMask()) && itemTemplate->Bonding == BIND_WHEN_PICKED_UP && !session->GetPlayer()->IsGameMaster())
                    continue;
                // Only display items in vendor lists for the team the
                // player is on. If GM on, display all items.
                if (!session->GetPlayer()->IsGameMaster() && ((itemTemplate->Flags2 & ITEM_FLAGS_EXTRA_HORDE_ONLY && session->GetPlayer()->GetTeamId() == TEAM_ALLIANCE) || (itemTemplate->Flags2 == ITEM_FLAGS_EXTRA_ALLIANCE_ONLY && session->GetPlayer()->GetTeamId() == TEAM_HORDE)))
                    continue;

                // Items sold out are not displayed in list
                uint32 leftInStock = !item->maxcount ? 0xFFFFFFFF : vendor->GetVendorItemCurrentCount(item);
                if (!session->GetPlayer()->IsGameMaster() && !leftInStock)
                    continue;

                ConditionList conditions = sConditionMgr->GetConditionsForNpcVendorEvent(vendor->GetEntry(), item->item);
                if (!sConditionMgr->IsObjectMeetToConditions(session->GetPlayer(), vendor, conditions))
                {
                    sLog->outError("SendListInventory: conditions not met for creature entry %u item %u", vendor->GetEntry(), item->item);
                    continue;
                }

                // reputation discount
                uint32 ExtendedToGold = item->ExtendedCost > extendedCostStartValue ? (item->ExtendedCost - extendedCostStartValue) * 10000 : 0;
                int32 price = item->IsGoldRequired(itemTemplate) ? uint32(floor(itemTemplate->BuyPrice * discountMod)) : ExtendedToGold;

                data << uint32(slot + 1);       // client expects counting to start at 1
                data << uint32(item->item);
                data << uint32(itemTemplate->DisplayInfoID);
                data << int32(leftInStock);
                data << uint32(price);
                data << uint32(itemTemplate->MaxDurability);
                data << uint32(itemTemplate->BuyCount);
                data << uint32(item->ExtendedCost);

                if (++count >= MAX_VENDOR_ITEMS)
                    break;
            }
        }
    }

    if (count == 0)
    {
        data << uint8(0);
        session->SendPacket(&data);
        return;
    }

    data.put<uint8>(countPos, count);
    session->SendPacket(&data);
}

bool HearthstoneMode::isInArray(int val)
{
    int SUPPORTED_CRITERIA[SUPPORTED_CRITERIA_NUMBER] = { 0,1,8,30,31,32,33,37,38,39,52,53,56,76,113 };
    int i;
    for (i = 0; i < SUPPORTED_CRITERIA_NUMBER; i++) {
        if (SUPPORTED_CRITERIA[i] == val)
            return true;
    }
    return false;
}

/*void HearthstoneMode::getItems()
{
items[0].clear();
items[1].clear();
items[2].clear();
items[3].clear();
items[4].clear();
items[5].clear();
items[6].clear();
items[7].clear();
QueryResult result = WorldDatabase.Query("SELECT entry, quality FROM item_template WHERE entry >= 100017 LIMIT 0, 200000");
do
{
Field* fields = result->Fetch();
uint32 entry = fields[0].GetUInt32();
uint32 quality = fields[1].GetUInt32();

items[quality].push_back(entry);
} while (result->NextRow());
}*/

int HearthstoneMode::getQuality()
{
    double c = rand_chance();
    float chance = (float)c;
    float i = CHANCES[0];
    int quality = 0;

    while (i < c)
    {
        quality = quality + 1;
        i = i + CHANCES[quality];
    }
    if (quality > 7)
        quality = QUALITY_TO_FILL_PERCENTAGE;
    return quality;
}

int HearthstoneMode::returnData0(AchievementCriteriaEntry const* criteria)
{
    int value = -1;
    switch (criteria->requiredType)
    {
    case 0:
        value = criteria->kill_creature.creatureID;
    case 1:
        value = criteria->win_bg.bgMapID;
    case 8:
        value = criteria->complete_achievement.linkedAchievement;
        break;
    case 30:
        value = criteria->bg_objective.objectiveId;
    case 31:
        value = criteria->honorable_kill_at_area.areaID;
        break;
    case 32: // win arena - no use of column 4
        break;
    case 33:
        value = criteria->play_arena.mapID;
        break;
    case 37: // win rated arena unsed column 4
        break;
    case 38:
        value = criteria->highest_team_rating.teamtype;
        break;
    case 39:
        // MISSING !!
        break;
    case 52:
        value = criteria->hk_class.classID;
        break;
    case 53:
        value = criteria->hk_race.raceID;
        break;
    case 56: // unused
        break;
    case 76: // unused
        break;
    case 113: //unused
        break;

    default:
        value = -1;

    }
    return value;
}

int HearthstoneMode::returnData1(AchievementCriteriaEntry const* criteria)
{
    int value = -1;
    switch (criteria->requiredType)
    {
    case 0:
        value = criteria->kill_creature.creatureCount;
    case 1:
        value = criteria->win_bg.winCount;
    case 8: // no column 5
        break;
    case 30:
        value = criteria->bg_objective.completeCount;
    case 31:
        value = criteria->honorable_kill_at_area.killCount;
        break;
    case 32: // win arena - no use of column 4
        break;
    case 33: // unused
        break;
    case 37: // win rated arena unsed column 4
        value = criteria->win_rated_arena.count;
        break;
    case 38: //unused
        break;
    case 39:
        // MISSING !!
        break;
    case 52:
        value = criteria->hk_class.count;
        break;
    case 53:
        value = criteria->hk_race.count;
        break;
    case 56:
        value = criteria->get_killing_blow.killCount;
        break;
    case 76:
        value = criteria->win_duel.duelCount;
        break;
    case 113:
        // MISSING !!
        break;

    default:
        value = -1;

    }
    return value;
}
/*
int WHISPER_CHANCE = 50;

std::vector<std::string> whispersList =
{
"Uccidere --NAME--, non sarà facile... buona fortuna!",
"Povero --NAME--... la sua fine è segnata!",
"--NAME--? Sarà un gioco da ragazzi ucciderlo.",
"Buona fortuna!",
"--NAME-- deve morire!"
};*/

/// ---------------- START OF SCRIPTS ------------------------- ///

#define GOSSIP_ITEM_GIVE_PVE_QUEST      "Vorrei ricevere la mia missione PVE giornaliera."
#define GOSSIP_ITEM_GIVE_PVP_QUEST      "Vorrei ricevere la mia missione PVP giornaliera."
#define GOSSIP_ITEM_GIVE_EXTRA_QUEST    "Vorrei una missione extra!" // unused
#define GOSSIP_ITEM_CHANGE_QUEST		"Vorrei cambiare la mia missione." // unused

class npc_han_al : public CreatureScript
{
public:
    npc_han_al() : CreatureScript("npc_han_al") { }
    /*
    void whisperPlayer(std::string creatureName, Player * player, Creature * creature)
    {
    double random = rand_chance();
    if (random <= WHISPER_CHANCE)
    {
    int index = rand() % whispersList.size() - 1;
    std::string whisperText = whispersList[index];
    replaceAll(whisperText, "--NAME--", creatureName);
    creature->Whisper(whisperText, LANG_UNIVERSAL, player, false);
    }
    }

    void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty())
    return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
    }
    }
    */
    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        time_t t = time(NULL);
        tm *lt = localtime(&t);
        int seed = lt->tm_mday + lt->tm_mon + 1 + lt->tm_year + 1900;
        srand(seed);
        //int id = rand() % PVE_RANGE;
        int id = PVE_LOWER_RANGE + (rand() % PVE_RANGE);
        Quest const * questPve = sObjectMgr->GetQuestTemplate(id);
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF:
            // uint16 id = (player->GetGUID().GetEntry() * id) % PVE_RANGE;

            if (!questPve)
                return false;

            if (player->CanAddQuest(questPve, false) && player->CanTakeQuest(questPve, false))
            {
                player->AddQuest(questPve, NULL);
                //CreatureTemplate const * objective = sObjectMgr->GetCreatureTemplate(questPve->RequiredNpcOrGo[0]);
                //whisperPlayer(objective->Name, player, creature);

                // simply close gossip or send something else?
                player->PlayerTalkClass->SendCloseGossip();
            }
            break;
        case GOSSIP_ACTION_INFO_DEF + 1:

            break;
        case GOSSIP_ACTION_INFO_DEF + 2:

            break;
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        unsigned char bitmask = 0;
        int gossip = 100000;

        time_t t = time(NULL);
        tm *lt = localtime(&t);
        int seed = lt->tm_mday + lt->tm_mon + 1 + lt->tm_year + 1900;
        srand(seed);
        //int id = rand() % PVE_RANGE;
        int idPve = PVE_LOWER_RANGE + (rand() % PVE_RANGE);
        Quest const * questPve = sObjectMgr->GetQuestTemplate(idPve);
        int PveMaxCheck = 0;
        int i = PVE_LOWER_RANGE;
        while (i <= PVE_UPPER_RANGE && PveMaxCheck <= MAX_PVE_QUEST_NUMBER)
        {
            if (player->GetQuestStatus(i) != QUEST_STATUS_NONE)
            {
                PveMaxCheck = PveMaxCheck + 1;
            }
            i = i + 1;
        }
        if (player->CanAddQuest(questPve, false) && player->CanTakeQuest(questPve, false) && PveMaxCheck < MAX_PVE_QUEST_NUMBER)
        {
            bitmask = bitmask | BITMASK_PVE;
        }

        if ((bitmask & BITMASK_PVE) == BITMASK_PVE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GIVE_PVE_QUEST, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        }
        if ((bitmask & BITMASK_PVP) == BITMASK_PVP)
        {
            // player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GIVE_PVP_QUEST, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        }
        if ((bitmask & BITMASK_EXTRA) == BITMASK_EXTRA)
        {
            // player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GIVE_EXTRA_QUEST, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        }
        if (bitmask == 0)
        {
            if (player->hasQuest(idPve)) // and id pvp and id extra
                gossip = 100001;
            else { gossip = 100002; }
        }
        if (PveMaxCheck >= MAX_PVE_QUEST_NUMBER)
            gossip = 100003;

        player->SEND_GOSSIP_MENU(gossip, creature->GetGUID());
        return true;
    }

    bool OnQuestComplete(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
    {
        return true;
    }
};

#define GOSSIP_ITEM_SHOW_ACCESS     "Vorrei vedere la tua merce, per favore."

class npc_azth_vendor : public CreatureScript
{
public:
    npc_azth_vendor() : CreatureScript("npc_azth_vendor") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_TRADE)
            sHearthstoneMode->AzthSendListInventory(creature->GetGUID(), player->GetSession(), 100000);

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        uint16 gossip;
        gossip = 32001;
        if (creature->GetResistance(SPELL_SCHOOL_HOLY) != 0)
            gossip = creature->GetResistance(SPELL_SCHOOL_HOLY);

        uint32 rep = player->GetReputation(AZTH_REPUTATION_ID);
        if (creature->IsVendor() && rep >= creature->GetResistance(SPELL_SCHOOL_FIRE) && (player->GetReputationRank(AZTH_REPUTATION_ID) >= 3))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_ITEM_SHOW_ACCESS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        if (player->GetReputation(AZTH_REPUTATION_ID) < creature->GetResistance(SPELL_SCHOOL_FIRE))
            player->SEND_GOSSIP_MENU(32002, creature->GetGUID());
        else
            player->SEND_GOSSIP_MENU(gossip, creature->GetGUID());

        return true;
    }
};

class item_azth_hearthstone_loot_sack : public ItemScript
{
public:
    item_azth_hearthstone_loot_sack() : ItemScript("item_azth_hearthstone_loot_sack") {}

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*target*/)
    {
        //sHearthstoneMode->getItems();
        SQLTransaction trans = CharacterDatabase.BeginTransaction();
        int16 deliverDelay = TIME_TO_RECEIVE_MAIL;
        MailDraft* draft = new MailDraft("Sacca Hearthstone", "");
        int i = 1;
        time_t t = time(NULL);
        tm *lt = localtime(&t);
        int seed = lt->tm_mday + lt->tm_mon + 1 + lt->tm_year + 1900 + lt->tm_sec + player->GetGUID() + player->GetItemCount(item->GetEntry(), true, 0);
        int loopCheck = 0;

        while (i <= EVERYTHING)
        {
            //srand(seed);
            int quality = 0;
            quality = sHearthstoneMode->getQuality();
            uint32 id = 0;
            Item* item;
            do
            {
                id = rand() % sHearthstoneMode->items[quality].size();
                item = Item::CreateItem(sHearthstoneMode->items[quality][id], 1, 0);
                loopCheck++;
            } while (!sHearthstoneMode->PlayerCanUseItem(item->GetTemplate(), player, true) && loopCheck < MAX_RETRY_GET_ITEM);

            if (item)
            {
                item->SaveToDB(trans);
                draft->AddItem(item);
            }

            i = i + 1;
            seed = seed + i;
        }
        i = 1;
        loopCheck = 0;
        while (i <= ONLY_COMMON)
        {
            //srand(seed + 3);
            int quality = 1;
            if (rand_chance() > 80)
            {
                quality = 0;
            }

            uint32 id = 0;
            Item* item;
            do
            {
                id = rand() % sHearthstoneMode->items[quality].size();
                item = Item::CreateItem(sHearthstoneMode->items[quality][id], 1, 0);
                loopCheck++;
            } while (!sHearthstoneMode->PlayerCanUseItem(item->GetTemplate(), player, true) && loopCheck < MAX_RETRY_GET_ITEM);
            if (item)
            {
                item->SaveToDB(trans);
                draft->AddItem(item);
            }

            i = i + 1;
            seed = seed + i;
        }
        i = 1;
        loopCheck = 0;
        while (i <= NOT_COMMON)
        {
            //srand(seed + 4);
            int quality = 0;
            quality = sHearthstoneMode->getQuality();
            while (quality < 3)
            {
                quality = sHearthstoneMode->getQuality();
            }
            uint32 id = 0;
            Item* item;
            do
            {
                id = rand() % sHearthstoneMode->items[quality].size();
                item = Item::CreateItem(sHearthstoneMode->items[quality][id], 1, 0);
                loopCheck++;
            } while (!sHearthstoneMode->PlayerCanUseItem(item->GetTemplate(), player, true) && loopCheck < MAX_RETRY_GET_ITEM);
            if (item)
            {
                item->SaveToDB(trans);
                draft->AddItem(item);
            }

            i = i + 1;
            seed = seed + i;
        }

        draft->SendMailTo(trans, MailReceiver(player), MailSender(player), MAIL_CHECK_MASK_RETURNED, deliverDelay);
        CharacterDatabase.CommitTransaction(trans);

        ChatHandler(player->GetSession()).SendSysMessage("Controlla la tua mail!");

        player->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);
        return true;
    }
};

bool HearthstoneMode::PlayerCanUseItem(ItemTemplate const* proto, Player* player, bool classCheck)
{
    if (proto)
    {
        if ((proto->Flags2 & ITEM_FLAGS_EXTRA_HORDE_ONLY) && player->GetTeamId(true) != TEAM_HORDE)
            return false;

        if ((proto->Flags2 & ITEM_FLAGS_EXTRA_ALLIANCE_ONLY) && player->GetTeamId(true) != TEAM_ALLIANCE)
            return false;

        if ((proto->AllowableClass & player->getClassMask()) == 0 || (proto->AllowableRace & player->getRaceMask()) == 0)
            return false;

        if (proto->RequiredSkill != 0)
        {
            if (player->GetSkillValue(proto->RequiredSkill) == 0)
                return false;
            else if (player->GetSkillValue(proto->RequiredSkill) < proto->RequiredSkillRank)
                return false;
        }

        if (proto->RequiredSpell != 0 && !player->HasSpell(proto->RequiredSpell))
            return false;

        if (player->getLevel() < proto->RequiredLevel)
            return false;

        const static uint32 item_weapon_skills[MAX_ITEM_SUBCLASS_WEAPON] =
        {
            SKILL_AXES,     SKILL_2H_AXES,  SKILL_BOWS,          SKILL_GUNS,      SKILL_MACES,
            SKILL_2H_MACES, SKILL_POLEARMS, SKILL_SWORDS,        SKILL_2H_SWORDS, 0,
            SKILL_STAVES,   0,              0,                   SKILL_FIST_WEAPONS,   0,
            SKILL_DAGGERS,  SKILL_THROWN,   SKILL_ASSASSINATION, SKILL_CROSSBOWS, SKILL_WANDS,
            SKILL_FISHING
        };

        if (classCheck && proto->Class == ITEM_CLASS_WEAPON && player->GetSkillValue(item_weapon_skills[proto->SubClass]) == 0)
            return false;

        if (classCheck && proto->Class == ITEM_CLASS_ARMOR)
        {
            uint32 type = proto->SubClass;
            uint32 pClass = player->getClass();

            switch (type)
            {
            case 1: //cloth
                if (pClass != CLASS_PRIEST && pClass != CLASS_MAGE && pClass != CLASS_WARLOCK)
                    return false;
                break;
            case 2: //leather
                if (pClass != CLASS_ROGUE && pClass != CLASS_DRUID)
                    return false;
                break;
            case 3: //mail
                if (pClass != CLASS_HUNTER && pClass != CLASS_SHAMAN)
                    return false;
                break;
            case 4: //plate
                if (pClass != CLASS_WARRIOR && pClass != CLASS_PALADIN && pClass != CLASS_DEATH_KNIGHT)
                    return false;
                break;
            default:
                return true;
            }
        }
        return true;
    }
    return false;
}


void HearthstoneMode::sendQuestCredit(Player *player, AchievementCriteriaEntry const* criteria)
{
    uint32 entry = 0;

    if (!isInArray(criteria->requiredType))
        return;

    uint32 achievementType = criteria->requiredType;

    // iterate through the loaded achievemements available
    for (std::vector<HearthstoneAchievement>::iterator itr = hsAchievementTable.begin(); itr != hsAchievementTable.end(); itr++)
    {
        if ((*itr).type == achievementType) // match the type
            if (((*itr).data0 == returnData0(criteria)) || ((*itr).data1 == returnData1(criteria))) // match criteria
            {
                entry = (*itr).creature; // set credit
                break;
            }
    }

    if (entry)
        player->azthPlayer->ForceKilledMonsterCredit(entry, NULL); // send credit
}

void HearthstoneMode::loadHearthstone()
{
    // initialize count and array
    uint32 count = 0;
    sHearthstoneMode->hsAchievementTable.clear();

    // run query
    QueryResult hsAchiResult = ExtraDatabase.PQuery("SELECT data0, data1, creature, type FROM hearthstone_criteria_credits");

    // store result in vector of hs achievement struct
    if (hsAchiResult)
    {
        do
        {
            HearthstoneAchievement ha = {};
            ha.data0 = (*hsAchiResult)[0].GetUInt32();
            ha.data1 = (*hsAchiResult)[1].GetUInt32();
            ha.creature = (*hsAchiResult)[2].GetUInt32();
            ha.type = (*hsAchiResult)[3].GetUInt32();

            sHearthstoneMode->hsAchievementTable.push_back(ha); // push the newly created element in the list
            count++;
        } while (hsAchiResult->NextRow());
    }

    // show log of loaded achievements at startup
    sLog->outError("Hearthstone Mode: loaded %u achievement definitions", count);

    int itemCount = 0;

    sHearthstoneMode->items[0].clear();
    sHearthstoneMode->items[1].clear();
    sHearthstoneMode->items[2].clear();
    sHearthstoneMode->items[3].clear();
    sHearthstoneMode->items[4].clear();
    sHearthstoneMode->items[5].clear();
    sHearthstoneMode->items[6].clear();
    sHearthstoneMode->items[7].clear();
    QueryResult result = ExtraDatabase.Query("SELECT entry FROM transmog_items WHERE entry >= 100017 LIMIT 0, 200000");
    do
    {
        Field* fields = result->Fetch();
        uint32 entry = fields[0].GetUInt32();
        uint32 quality = sObjectMgr->GetItemTemplate(entry)->Quality; //= fields[1].GetUInt32();

        sHearthstoneMode->items[quality].push_back(entry);
        itemCount++;
    } while (result->NextRow());

    sLog->outError("Hearthstone Mode: loaded %u transmog items", itemCount);
}

void AddSC_hearthstone()
{
    new npc_han_al();
    new npc_azth_vendor();
    new item_azth_hearthstone_loot_sack();
    //new azth_hearthstone_world();
}

