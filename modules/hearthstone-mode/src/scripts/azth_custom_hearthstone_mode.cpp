// Copyright (c) 2016 AzerothCore
// Author: Gargarensis
// Refactored by: mik1893 
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.

#include "azth_custom_hearthstone_mode.h"
#include <iostream>
#include <cstdlib> // now using C++ header
#include "AzthPlayer.h"

// old
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

                // the no-patch extended cost (shown in gold)
                //uint32 ExtendedToGold = item->ExtendedCost > extendedCostStartValue ? (item->ExtendedCost - extendedCostStartValue) * 10000 : 0;
                //int32 price = item->IsGoldRequired(itemTemplate) ? uint32(floor(itemTemplate->BuyPrice * discountMod)) : ExtendedToGold;

                int32 price = item->IsGoldRequired(itemTemplate) ? uint32(floor(itemTemplate->BuyPrice * discountMod)) : 0;


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

/// ---------------- START OF SCRIPTS ------------------------- ///

#define GOSSIP_ITEM_GIVE_PVE_QUEST      "Vorrei ricevere la mia missione PVE giornaliera."
#define GOSSIP_ITEM_GIVE_PVP_QUEST      "Vorrei ricevere la mia missione PVP giornaliera."

class npc_han_al : public CreatureScript
{
public:
    uint32 pveId = 0;
    uint32 pvpId = 0;
    npc_han_al() : CreatureScript("npc_han_al") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        if (pveId == 0 && pvpId == 0)
            return false;

        Quest const * questPve = sObjectMgr->GetQuestTemplate(pveId);
        Quest const * questPvp = sObjectMgr->GetQuestTemplate(pvpId);
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF:

            if (!questPve)
                return false;

            if (player->CanAddQuest(questPve, false) && player->CanTakeQuest(questPve, false))
            {
                player->AddQuest(questPve, NULL);

                // simply close gossip or send something else?
                player->PlayerTalkClass->SendCloseGossip();
            }
            break;
        case GOSSIP_ACTION_INFO_DEF + 1:

            if (!questPvp)
                return false;

            if (player->CanAddQuest(questPvp, false) && player->CanTakeQuest(questPvp, false))
            {
                player->AddQuest(questPvp, NULL);

                // simply close gossip or send something else?
                player->PlayerTalkClass->SendCloseGossip();
            }
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:

            break;
        }
        player->PlayerTalkClass->SendCloseGossip();
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
        pveId = sHearthstoneMode->hsPveQuests.at(rand() % (sHearthstoneMode->hsPveQuests.size() - 1)).id;
        Quest const * questPve = sObjectMgr->GetQuestTemplate(pveId);

        t = time(NULL);
        seed = lt->tm_mday + lt->tm_mon + 1 + lt->tm_year + 1900 + player->GetGUID();
        srand(seed);
        pvpId = sHearthstoneMode->hsPvpQuests.at(rand() % (sHearthstoneMode->hsPvpQuests.size() - 1)).id;
        Quest const * questPvp = sObjectMgr->GetQuestTemplate(pvpId);

#pragma region "Pve Quest Check"
        int PveMaxCheck = 0;
        int i = 0;
        while (i <= (sHearthstoneMode->hsPveQuests.size() - 1) && PveMaxCheck <= MAX_PVE_QUEST_NUMBER)
        {
            if (player->GetQuestStatus(sHearthstoneMode->hsPveQuests.at(i).id) != QUEST_STATUS_NONE)
            {
                PveMaxCheck = PveMaxCheck + 1;
            }
            i = i + 1;
        }
        if (questPve && player->CanAddQuest(questPve, false) && player->CanTakeQuest(questPve, false) && PveMaxCheck < MAX_PVE_QUEST_NUMBER)
        {
            bitmask = bitmask | BITMASK_PVE;
        }
#pragma endregion

#pragma region "Pvp Quest Check"
        int PvpMaxCheck = 0;
        i = 0;
        while (i <= (sHearthstoneMode->hsPvpQuests.size() - 1) && PvpMaxCheck <= MAX_PVE_QUEST_NUMBER) // NEED PVP MAX?
        {
            if (player->GetQuestStatus(sHearthstoneMode->hsPvpQuests.at(i).id) != QUEST_STATUS_NONE)
            {
                PvpMaxCheck = PvpMaxCheck + 1;
            }
            i = i + 1;
        }
        if (questPvp && player->CanAddQuest(questPvp, false) && player->CanTakeQuest(questPvp, false) && PvpMaxCheck < MAX_PVP_QUEST_NUMBER)
        {
            bitmask = bitmask | BITMASK_PVP;
        }
#pragma endregion

        if ((bitmask & BITMASK_PVE) == BITMASK_PVE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GIVE_PVE_QUEST, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        }
        if ((bitmask & BITMASK_PVP) == BITMASK_PVP)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GIVE_PVP_QUEST, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        }
        if ((bitmask & BITMASK_EXTRA) == BITMASK_EXTRA)
        {
            // player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GIVE_EXTRA_QUEST, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        }
        if (bitmask == 0)
        {
            if (player->hasQuest(pveId) && player->hasQuest(pvpId)) // and id extra
                gossip = 100001;
            else { gossip = 100002; }
        }
        if (PveMaxCheck >= MAX_PVE_QUEST_NUMBER && PvpMaxCheck >= MAX_PVP_QUEST_NUMBER)
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
        std::vector<HearthstoneVendor> vendors = sHearthstoneMode->hsVendors;
        int pos = 0;
        for (int i = 0; i < vendors.size(); i++)
        {
            HearthstoneVendor temp = vendors.at(i);
            if (temp.id == creature->GetEntry())
                pos = i;
        }

        if (pos == 0)
            return false;
        
        HearthstoneVendor vendor = vendors.at(pos);
        
        if (vendor.pvpVendor && !player->azthPlayer->isPvP() && !player->IsGameMaster())
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT,  "Devi essere un player Full PvP per accedere a questo vendor.", GOSSIP_SENDER_MAIN, 0);
            player->SEND_GOSSIP_MENU(vendor.gossipNope, creature->GetGUID());
            return true;
        }


        uint16 gossip;
        
        if (vendor.reputationId < 0)
        {
            int16 bracket = vendor.reputationId * -1;
            int32 rating = player->GetArenaPersonalRating(ArenaTeam::GetSlotByType(bracket));
            
            if (!creature->IsVendor() || rating < vendor.repValue) {
                stringstream ss;
                ss << vendor.repValue;
                
                std::string str="Hai bisogno di "+ss.str()+" personal rating ";
                
                switch(bracket) {
                    case 1:
                        str+" 1v1";
                    break;
                    case 2:
                        str+" 2v2";
                    break;
                    case 3:
                        str+" 3v3";
                    break;
                    case 4:
                        str+" 3v3 Solo Queue";
                    break;
                    case 5:
                        str+" 5v5";
                    break;
                }

                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, str, GOSSIP_SENDER_MAIN, 0);
                player->SEND_GOSSIP_MENU(vendor.gossipNope, creature->GetGUID());
            } else {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_ITEM_SHOW_ACCESS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
                player->SEND_GOSSIP_MENU(vendor.gossipOk, creature->GetGUID());
            }
            return true;
        }
        
        // REPUTATION VENDOR
        int32 rep = player->GetReputation(vendor.reputationId);

        if (creature->IsVendor() && rep >= vendor.repValue && (player->GetReputationRank(vendor.reputationId) >= 3))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_ITEM_SHOW_ACCESS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        if (rep < vendor.repValue) {
            stringstream ss;
            ss << vendor.repValue;
            std::string str = "Hai bisogno di " + ss.str() + " reputazione con AzerothShard.";

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, str, GOSSIP_SENDER_MAIN, 0);

            player->SEND_GOSSIP_MENU(vendor.gossipNope, creature->GetGUID());
        }
        else
            player->SEND_GOSSIP_MENU(vendor.gossipOk, creature->GetGUID());
        
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

        Item* newItem = Item::CreateItem(100017, 1, 0);

        while (i <= EVERYTHING)
        {
            //srand(seed);
            int quality = 0;
            quality = sHearthstoneMode->getQuality();
            uint32 id = 0;
            do
            {
                if (sHearthstoneMode->items[quality].size() != 0)
                {
                    id = rand() % sHearthstoneMode->items[quality].size();
                    newItem = Item::CreateItem(sHearthstoneMode->items[quality][id], 1, 0);
                }
                loopCheck++;
            } while (!sHearthstoneMode->PlayerCanUseItem(newItem, player, true) && loopCheck < MAX_RETRY_GET_ITEM);

            if (newItem)
            {
                newItem->SaveToDB(trans);
                draft->AddItem(newItem);
            }

            i = i + 1;
            seed = seed + i;
        }
        i = 1;
        loopCheck = 0;
        newItem = Item::CreateItem(100017, 1, 0);
        while (i <= ONLY_COMMON)
        {
            //srand(seed + 3);
            int quality = 1;
            if (rand_chance() > 80)
            {
                quality = 0;
            }

            uint32 id = 0;
            do
            {
                if (sHearthstoneMode->items[quality].size() != 0)
                {
                    id = rand() % sHearthstoneMode->items[quality].size();
                    newItem = Item::CreateItem(sHearthstoneMode->items[quality][id], 1, 0);
                }
                loopCheck++;
            } while (!sHearthstoneMode->PlayerCanUseItem(newItem, player, true) && loopCheck < MAX_RETRY_GET_ITEM);
            if (newItem)
            {
                newItem->SaveToDB(trans);
                draft->AddItem(newItem);
            }

            i = i + 1;
            seed = seed + i;
        }
        i = 1;
        loopCheck = 0;
        newItem = Item::CreateItem(100017, 1, 0);
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
            do
            {
                if (sHearthstoneMode->items[quality].size() != 0)
                {
                    id = rand() % sHearthstoneMode->items[quality].size();
                    newItem = Item::CreateItem(sHearthstoneMode->items[quality][id], 1, 0);
                }
                loopCheck++;
            } while (!sHearthstoneMode->PlayerCanUseItem(newItem, player, true) && loopCheck < MAX_RETRY_GET_ITEM);
            if (newItem)
            {
                newItem->SaveToDB(trans);
                draft->AddItem(newItem);
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

bool HearthstoneMode::PlayerCanUseItem(Item const* item, Player* player, bool classCheck)
{
    if (!item)
        return false;

    ItemTemplate const* proto = item->GetTemplate();

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


void HearthstoneMode::sendQuestCredit(Player *player, AchievementCriteriaEntry const* criteria, std::vector<uint32>& hsCheckList)
{
    uint32 entry = 0;

    if (!isInArray(criteria->requiredType))
        return;

    uint32 achievementType = criteria->requiredType;

    // iterate through the loaded achievemements available
    for (std::vector<HearthstoneAchievement>::iterator itr = hsAchievementTable.begin(); itr != hsAchievementTable.end(); itr++)
    {
        if ((*itr).type == achievementType) // match the type
            if ((*itr).data0 == returnData0(criteria) && criteria->timerStartEvent == 0) // match criteria
            {
                entry = (*itr).creature; // set credit
                break;
            }
    }
    

/*	for (int i = 0; i < hsAchievementTable.size(); i++)
	{
		if (hsAchievementTable[i].type == achievementType)
			if 
	}*/

    if (entry != 0 && std::find(hsCheckList.begin(), hsCheckList.end(), entry) == hsCheckList.end()) {
        hsCheckList.push_back(entry);
        player->azthPlayer->ForceKilledMonsterCredit(entry, NULL); // send credit   
    }
}

int64 HearthstoneMode::getHeartstoneQuestFlag(uint64 id)
{
	for (std::vector<HearthstoneQuest>::iterator itr = hsPveQuests.begin(); itr != hsPveQuests.end(); itr++)
	{
		if ((*itr).id == id)
			return (*itr).flag;
	}
	for (std::vector<HearthstoneQuest>::iterator itr = hsPvpQuests.begin(); itr != hsPvpQuests.end(); itr++)
	{
		if ((*itr).id == id)
			return (*itr).flag;
	}
	return -1;
};

void HearthstoneMode::loadHearthstone()
{
    uint32 count = 0;
    sHearthstoneMode->hsAchievementTable.clear();

    QueryResult hsAchiResult = ExtraDatabase.PQuery("SELECT data0, data1, creature, type FROM hearthstone_criteria_credits");

    if (hsAchiResult)
    {
        do
        {
            HearthstoneAchievement ha = {};
            ha.data0 = (*hsAchiResult)[0].GetUInt32();
            ha.data1 = (*hsAchiResult)[1].GetUInt32();
            ha.creature = (*hsAchiResult)[2].GetUInt32();
            ha.type = (*hsAchiResult)[3].GetUInt32();

            sHearthstoneMode->hsAchievementTable.push_back(ha);
            count++;
        } while (hsAchiResult->NextRow());
    }

    sLog->outString("Hearthstone Mode: loaded %u achievement definitions", count);

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

    sLog->outString("Hearthstone Mode: loaded %u transmog items", itemCount);

        uint32 questCount = 0;
        sHearthstoneMode->hsPveQuests.clear();
        sHearthstoneMode->hsPvpQuests.clear();

        QueryResult hsQuestResult = ExtraDatabase.PQuery("SELECT id, flag FROM hearthstone_quests");

        if (hsQuestResult)
        {
            do
            {
                HearthstoneQuest hq = {};
                hq.id = (*hsQuestResult)[0].GetUInt32();
                hq.flag = (*hsQuestResult)[1].GetUInt32();
                unsigned char bitmask = hq.flag;

                if ((bitmask & BITMASK_PVE) == BITMASK_PVE)
                    sHearthstoneMode->hsPveQuests.push_back(hq); // push the newly created element in the list
                if ((bitmask & BITMASK_PVP) == BITMASK_PVP)
                    sHearthstoneMode->hsPvpQuests.push_back(hq); // push the newly created element in the list

                questCount++;
            } while (hsQuestResult->NextRow());
        }
    
    sLog->outString("Hearthstone Mode: loaded %u quests", questCount);


    uint32 vendorCount = 0;
    sHearthstoneMode->hsVendors.clear();

    QueryResult hsVendorResult = ExtraDatabase.PQuery("SELECT vendorId, reputation, `value`, gossipSatisfied, gossipUnsatisfied, PvPVendor FROM reputation_vendor");

    if (hsVendorResult)
    {
        do
        {
            HearthstoneVendor hv = {};
            hv.id = (*hsVendorResult)[0].GetUInt32();
            hv.reputationId = (*hsVendorResult)[1].GetInt32();
            hv.repValue = (*hsVendorResult)[2].GetInt32();
            hv.gossipOk = (*hsVendorResult)[3].GetUInt32();
            hv.gossipNope = (*hsVendorResult)[4].GetUInt32();            
            ((*hsVendorResult)[5].GetUInt32() == 1) ? (hv.pvpVendor = true) : (hv.pvpVendor = false); //if == 1 ->pvp vendor || else-> non pvp vendor

            sHearthstoneMode->hsVendors.push_back(hv); // push the newly created element in the list

            vendorCount++;
        } while (hsVendorResult->NextRow());
    }

    sLog->outString("Hearthstone Mode: loaded %u vendors", vendorCount);
}

void AddSC_hearthstone()
{
    new npc_han_al();
    new npc_azth_vendor();
    new item_azth_hearthstone_loot_sack();
    //new azth_hearthstone_world();
}

