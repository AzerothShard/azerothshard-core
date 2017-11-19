#include "ScriptMgr.h"
#include "Unit.h"
#include "SharedDefines.h"
#include "Player.h"
#include "Common.h"
#include "AzthPlayer.h"
#include "AzthUtils.h"
#include "AzthGearScaling.h"
#include "AzthGearScalingSocket.h"
#include "AzthLanguageStrings.h"
#include "ScriptedGossip.h"


#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

std::map<uint32, AzthGearScaling> tournamentTempGearList;
std::map<uint64, AzthGearScalingSocket> tournamentTempGearSocketList;

class loadTournamentSet : public WorldScript
{
public:
    loadTournamentSet() : WorldScript("loadTournamentSet") {}
    void OnStartup() override
    {
        QueryResult tournament_tempGear_table = ExtraDatabase.PQuery("SELECT * FROM tournament_set");
        if (!tournament_tempGear_table)
        {
            sLog->outString(">> Loaded 0 Temp Gear for Tournament Set. DB table `tournament_set` is empty.\n");
            return;
        }

        Field* tournament_tempGear_Field = tournament_tempGear_table->Fetch();
        do
        {
            //                                    season*10000                                  class*100                                           spec                                                        season                               class                                           spec                                 name                                                   head                                       neck                                      shoulders                                   body                                 chest                                         waist                                  legs                                       feet                                        wrists                                    wrists_h                                   hands                                       finger1                                finger1_h                                    finger2                                      finger2_h                                  trinket1                                  trinket1_h                               trinket2                                    trinket2_h                              back                                             mainHand                                    offhand                                   ranged                                  tabard
            tournamentTempGearList[tournament_tempGear_Field[0].GetUInt32() * 10000 + tournament_tempGear_Field[1].GetUInt32() * 100 + tournament_tempGear_Field[2].GetUInt32()] = AzthGearScaling(tournament_tempGear_Field[0].GetUInt32(), tournament_tempGear_Field[1].GetUInt32(), tournament_tempGear_Field[2].GetUInt32(), tournament_tempGear_Field[3].GetString().c_str(), tournament_tempGear_Field[4].GetUInt32(), tournament_tempGear_Field[5].GetUInt32(), tournament_tempGear_Field[6].GetUInt32(), tournament_tempGear_Field[7].GetUInt32(), tournament_tempGear_Field[8].GetUInt32(), tournament_tempGear_Field[9].GetUInt32(), tournament_tempGear_Field[10].GetUInt32(), tournament_tempGear_Field[11].GetUInt32(), tournament_tempGear_Field[12].GetUInt32(), tournament_tempGear_Field[13].GetUInt32(), tournament_tempGear_Field[14].GetUInt32(), tournament_tempGear_Field[15].GetUInt32(), tournament_tempGear_Field[16].GetUInt32(), tournament_tempGear_Field[17].GetUInt32(), tournament_tempGear_Field[18].GetUInt32(), tournament_tempGear_Field[19].GetUInt32(), tournament_tempGear_Field[20].GetUInt32(), tournament_tempGear_Field[21].GetUInt32(), tournament_tempGear_Field[22].GetUInt32(), tournament_tempGear_Field[23].GetUInt32(), tournament_tempGear_Field[24].GetUInt32(), tournament_tempGear_Field[25].GetUInt32(), tournament_tempGear_Field[26].GetUInt32(), tournament_tempGear_Field[27].GetUInt32());
        } while (tournament_tempGear_table->NextRow());

        sAzthGearScalingMgr->SetGearScalingList(tournamentTempGearList);

        //-------------------------------------------------------------------------------

        QueryResult tournament_socketEnchants_table = ExtraDatabase.PQuery("SELECT itemId,class,spec,socket1,socket2,socket3,enchant FROM tournament_socket_enchants");
        if (!tournament_socketEnchants_table)
        {
            sLog->outString(">> Loaded 0 tournament Gear Socket for Tournament Set. DB table `tournament_socket_enchants` is empty.\n");
        }

        else
        {
            Field* tournament_socketEnchants_Field = tournament_socketEnchants_table->Fetch();
            do
            {
                //                                  entry*10000                                             class*100                                                spec 
                tournamentTempGearSocketList[tournament_socketEnchants_Field[0].GetUInt32() * 10000 + tournament_socketEnchants_Field[1].GetUInt32() * 100 + tournament_socketEnchants_Field[2].GetUInt32()] = AzthGearScalingSocket(
                    tournament_socketEnchants_Field[0].GetUInt32(), // entry
                    tournament_socketEnchants_Field[1].GetUInt8(), // class
                    tournament_socketEnchants_Field[2].GetUInt8(), // spec
                    tournament_socketEnchants_Field[3].GetUInt32(), // socket1
                    tournament_socketEnchants_Field[4].GetUInt32(), // socket2
                    tournament_socketEnchants_Field[5].GetUInt32(), // socket3
                    tournament_socketEnchants_Field[6].GetUInt32()); // enchant
            } while (tournament_socketEnchants_table->NextRow());

            sAzthGearScalingSocketMgr->SetGearScalingSocketList(tournamentTempGearSocketList);
        }
    }
       
};

class tournamentGossip : public CreatureScript
{
public:
    tournamentGossip() : CreatureScript("tournamentGossip") {}

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->IsInCombat())
        {
            ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000You are in combat|r");
            return true;
        }
        
        if (player->getLevel()>=80) {
            if (!player->azthPlayer->hasGear())
            {
                string str = sAzthLang->get(AZTH_LANG_PVP_NPC_SET_ADVICE,player);
                player->ADD_GOSSIP_ITEM_EXTENDED(1, "Set PvP: Deadly", GOSSIP_SENDER_MAIN, 5, str, 0, false);
                player->ADD_GOSSIP_ITEM_EXTENDED(1, "Set PvP: Furious", GOSSIP_SENDER_MAIN, 6, str, 0, false);
                player->ADD_GOSSIP_ITEM_EXTENDED(1, "Set PvP: Relentless", GOSSIP_SENDER_MAIN, 7, str, 0, false);
                //player->ADD_GOSSIP_ITEM_EXTENDED(0, "Wrathful", GOSSIP_SENDER_MAIN, 8, str.c_str(), 0, false);
            }
            else
                player->ADD_GOSSIP_ITEM(0, "Rimuovi il set PVP", GOSSIP_SENDER_MAIN, 9);
        }

        if (player->azthPlayer->isPvP()) {
            player->ADD_GOSSIP_ITEM(8, "Gratis: Ripara tutti gli items", GOSSIP_SENDER_MAIN, 20);
            player->ADD_GOSSIP_ITEM_EXTENDED(3, "Gratis: Resetta i talenti", GOSSIP_SENDER_MAIN, 21, "Vuoi davvero resettare tutti i talenti?", 0, false);
        }

        player->SEND_GOSSIP_MENU(1, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32  /*sender*/, uint32 action)
    {
        if (action >= 4 && action <= 8) // deadly = 5, furious = 6, relentless = 7, wrathful = 8 -> go to select spec menu
            selectSpec(player, creature, action);
        
        if (action == 20 && player->azthPlayer->isPvP()) {
            player->DurabilityRepairAll(false,0,false);
            player->PlayerTalkClass->SendCloseGossip();
        }
        
        if (action == 21 && player->azthPlayer->isPvP()) {
            player->resetTalents(true);
            player->SendTalentsInfoData(false);
            creature->CastSpell(player, 14867, true);                  //spell: "Untalent Visual Effect"
            player->PlayerTalkClass->SendCloseGossip();
        }

        if (action >= 10000) //spec selected -> equip new gear
        {
            uint32 season = action / 10000;
            uint32 spec = action - (season * 10000) - (player->getClass()*100);
            AzthGearScaling & set = sAzthGearScalingMgr->GetGearScalingList()[action];

            if (equipSet(creature, set, player, spec)) {
                if (!player->azthPlayer->isPvP()) {
                    QueryResult PVPSetCharactersActive_table = CharacterDatabase.PQuery(("INSERT IGNORE INTO azth_tournamentset_active (`id`, `season`, `spec`) VALUES ('%d', '%d', '%d');"), player->GetGUID(), season, spec);
                }

                player->SaveToDB(false, false);
            }

            player->PlayerTalkClass->SendCloseGossip();
        }

        if (action == 9) //exit from this mode
        {
            for (uint32 INVENTORY_INDEX = EQUIPMENT_SLOT_START; INVENTORY_INDEX < EQUIPMENT_SLOT_END; INVENTORY_INDEX++)
            {
                Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, INVENTORY_INDEX);
                if (item != nullptr)
                {
                    player->DestroyItem(INVENTORY_SLOT_BAG_0, INVENTORY_INDEX, true);
                }
            }
            player->azthPlayer->SetTempGear(false);
            QueryResult PVPSetCharactersActive_table = CharacterDatabase.PQuery(("DELETE FROM azth_tournamentset_active WHERE  `id`=%d;"), player->GetGUID());
            player->SaveToDB(false, false);
            ChatHandler(player->GetSession()).PSendSysMessage("Il set pvp tournament e' stato rimosso");
            player->PlayerTalkClass->SendCloseGossip();
        }

        return true;
    }

    void selectSpec(Player* player, Creature* creature, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        uint32 season = action;

        for (std::map<uint32, AzthGearScaling>::iterator it = tournamentTempGearList.begin(); it != tournamentTempGearList.end(); it++)
        {
            if (it->second.GetClass() == player->getClass() && it->second.GetSeason() == season)
            {
                //season*10000 + class*100 + spec
                player->ADD_GOSSIP_ITEM(8, it->second.GetName().c_str(), GOSSIP_SENDER_MAIN, season * 10000 + it->second.GetClass() * 100 + it->second.GetSpec());
            }
        }
        player->SEND_GOSSIP_MENU(1, creature->GetGUID());
    }

    void unequipItem(Player* player, uint32 invIndex) {      
        Item *item=player->GetItemByPos(INVENTORY_SLOT_BAG_0, invIndex);
        
        if (item == nullptr)
            return;
        
        ItemPosCountVec off_dest;
        uint8 off_msg = player->CanStoreItem(NULL_BAG, NULL_SLOT, off_dest, item, false);
        if (off_msg == EQUIP_ERR_OK)
        {
            player->RemoveItem(INVENTORY_SLOT_BAG_0, invIndex, true);
            player->StoreItem(off_dest, item, true);
        }
    }
    
    uint32 getFactionItem(uint32 entry, Player *player) {
        // this change automatically items of all slots depending by team
        // "_h" field instead can be used to differentiate (or deprecated)
        if (player->GetTeamId(true) == TEAM_HORDE) {
            if (sObjectMgr->FactionChangeItems.find(entry) != sObjectMgr->FactionChangeItems.end()) {
                uint32 horde=sObjectMgr->FactionChangeItems[entry];
                if (horde)
                    entry=horde;
            }
        } else { // ally
            if (sAzthUtils->FactionChangeItemsHorde.find(entry) != sAzthUtils->FactionChangeItemsHorde.end()) {
                uint32 ally=sAzthUtils->FactionChangeItemsHorde[entry];
                if (ally)
                    entry=ally;
            }
        }
        
        return entry;
    }
    
    void equipItem(uint32 entry, uint16 slot, Player *player, uint32 spec, bool update = true) {
        if (!entry)
            return;
        
        uint32 originalEntry=entry;
        
        entry=getFactionItem(entry, player);

        
        /* // already done in preCheckEquip
         * Item* pItem = Item::CreateItem(entry, 1, player);
        
        if (player->azthPlayer->isPvP()) {
            InventoryResult msg = player->CanEquipItem(INVENTORY_SLOT_BAG_0, slot, pItem, false);
            if (msg != EQUIP_ERR_OK)
            {
                player->SendEquipError(msg, pItem, NULL);
                ChatHandler(player->GetSession()).PSendSysMessage("%s", sAzthLang->get(AZTH_LANG_PVP_NPC_CANNOT_EQUIP));
                return;
            }
        }*/
        
        Item* item = player->EquipNewItem(slot, entry, update);

        //soulbound with our special flag
        item->SetBinding(true);
        item->ApplyModFlag(ITEM_FIELD_FLAGS, ITEM_FLAG_UNK1, true);

        setEnchantAndSocket(player, item, originalEntry, spec);
    }
    
    void preCheckEquip(uint32 entry, uint16 slot, Player *player, bool &ok) {
        if (!entry)
            return;
        
        entry=getFactionItem(entry, player);

        Item* pItem = Item::CreateItem(entry, 1, player);
        
        InventoryResult msg = player->CanEquipItem(INVENTORY_SLOT_BAG_0, slot, pItem, true); // must be considered a swap
        if (msg != EQUIP_ERR_OK 
            && msg != EQUIP_ERR_CANT_EQUIP_WITH_TWOHANDED) // TODO: maybe two hand control skip should be solved in another way. It can skip some important checks.
        {
            if (player->azthPlayer->isPvP() 
                || msg != EQUIP_ERR_CANT_CARRY_MORE_OF_THIS) {
                player->SendEquipError(msg, pItem, NULL);
                const char *msg=sAzthLang->getf(AZTH_LANG_PVP_NPC_CANNOT_EQUIP, player, entry, pItem->GetTemplate()->Name1.c_str());
                ChatHandler(player->GetSession()).SendSysMessage(msg);
                ok = false;
            }
        }
    }

    bool equipSet(Creature *creature, AzthGearScaling set, Player* player, uint32 spec)
    {       
        bool ok=true;
        preCheckEquip(set.GetHead(), SLOT_HEAD, player, ok);
        preCheckEquip(set.GetNeck(), SLOT_NECK, player, ok);
        preCheckEquip(set.GetShoulders(), SLOT_SHOULDERS, player, ok);
        preCheckEquip(set.GetBody(), SLOT_SHIRT, player, ok);
        preCheckEquip(set.GetChest(), SLOT_CHEST, player, ok);
        preCheckEquip(set.GetWaist(), SLOT_WAIST, player, ok);
        preCheckEquip(set.GetLegs(), SLOT_LEGS, player, ok);
        preCheckEquip(set.GetFeet(), SLOT_FEET, player, ok);
        preCheckEquip(set.GetHands(), SLOT_HANDS, player, ok);
        preCheckEquip(set.GetBack(), SLOT_BACK, player, ok);
        preCheckEquip(set.GetMainHand(), SLOT_MAIN_HAND, player, ok);
        preCheckEquip(set.GetOffHand(), SLOT_OFF_HAND, player, ok);
        preCheckEquip(set.GetRanged(), SLOT_RANGED, player, ok);
        preCheckEquip(set.GetTabard(), SLOT_TABARD, player, ok);

        if (player->GetTeamId(true) == TEAM_ALLIANCE || set.GetWrists_h() == 0)
        {
            preCheckEquip(set.GetWrists(), SLOT_WRISTS, player, ok);
        }
        else
        {
            preCheckEquip(set.GetWrists_h(), SLOT_WRISTS, player, ok);
        }
        
        if (player->GetTeamId(true) == TEAM_ALLIANCE || set.GetFinger1_h() == 0)
        {
            preCheckEquip(set.GetFinger1(), SLOT_FINGER1, player, ok);
        }
        else
        {
            preCheckEquip(set.GetFinger1_h(), SLOT_FINGER1, player, ok);
        }

        if (player->GetTeamId(true) == TEAM_ALLIANCE || set.GetFinger2_h() == 0)
        {
            preCheckEquip(set.GetFinger2(), SLOT_FINGER2, player, ok);
        }
        else
        {
            preCheckEquip(set.GetFinger2_h(), SLOT_FINGER2, player, ok);
        }
        
        if (player->GetTeamId(true) == TEAM_ALLIANCE || set.GetTrinket1_h() == 0)
        {
            preCheckEquip(set.GetTrinket1(), SLOT_TRINKET1, player, ok);
        }
        else
        {
            preCheckEquip(set.GetTrinket1_h(), SLOT_TRINKET1, player, ok);
        }
        
        if (player->GetTeamId(true) == TEAM_ALLIANCE || set.GetTrinket2_h() == 0)
        {
            preCheckEquip(set.GetTrinket2(), SLOT_TRINKET2, player, ok);
        }
        else
        {
            preCheckEquip(set.GetTrinket2_h(), SLOT_TRINKET2, player, ok);
        }
        
        if (!ok) {
            return false;
        }

        uint32 spaceCnt=sAzthUtils->getFreeSpaceInBags(player);
        uint32 equipCnt=0;

        for (uint32 INVENTORY_INDEX = EQUIPMENT_SLOT_START; INVENTORY_INDEX < EQUIPMENT_SLOT_END; INVENTORY_INDEX++) {
            Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, INVENTORY_INDEX);

            if (item == nullptr)
                continue;
            
            equipCnt++;

            InventoryResult off_msg = player->CanUnequipItem(item->GetPos(), false);
            if (off_msg != EQUIP_ERR_OK && off_msg != EQUIP_ERR_CAN_ONLY_DO_WITH_EMPTY_BAGS)
                return false;
        }
        
        if (spaceCnt < equipCnt) {
            std::string str = "You have only "+std::to_string(spaceCnt)+" free space in your bags, at least "+std::to_string(equipCnt)+" are needed";
            creature->MonsterWhisper(str.c_str(), player);
            return false;
        }

        //remove equipped items and send to mail
        for (uint32 INVENTORY_INDEX = EQUIPMENT_SLOT_START; INVENTORY_INDEX < EQUIPMENT_SLOT_END; INVENTORY_INDEX++)
        {
            unequipItem(player, INVENTORY_INDEX);
        }      

        player->RemoveItemDependentAurasAndCasts((Item*)NULL);

        if (!player->azthPlayer->isPvP())
            player->azthPlayer->SetTempGear(true);
        
        equipItem(set.GetHead(), SLOT_HEAD, player, spec);
        equipItem(set.GetNeck(), SLOT_NECK, player, spec);
        equipItem(set.GetShoulders(), SLOT_SHOULDERS, player, spec);
        equipItem(set.GetBody(), SLOT_SHIRT, player, spec);
        equipItem(set.GetChest(), SLOT_CHEST, player, spec);
        equipItem(set.GetWaist(), SLOT_WAIST, player, spec);
        equipItem(set.GetLegs(), SLOT_LEGS, player, spec);
        equipItem(set.GetFeet(), SLOT_FEET, player, spec);
        equipItem(set.GetHands(), SLOT_HANDS, player, spec);
        equipItem(set.GetBack(), SLOT_BACK, player, spec);
        equipItem(set.GetMainHand(), SLOT_MAIN_HAND, player, spec);
        equipItem(set.GetOffHand(), SLOT_OFF_HAND, player, spec);
        equipItem(set.GetRanged(), SLOT_RANGED, player, spec);
        equipItem(set.GetTabard(), SLOT_TABARD, player, spec);

        if (player->GetTeamId(true) == TEAM_ALLIANCE || set.GetWrists_h() == 0)
        {
            equipItem(set.GetWrists(), SLOT_WRISTS, player, spec);
        }
        else
        {
            equipItem(set.GetWrists_h(), SLOT_WRISTS, player, spec);
        }
        
        if (player->GetTeamId(true) == TEAM_ALLIANCE || set.GetFinger1_h() == 0)
        {
            equipItem(set.GetFinger1(), SLOT_FINGER1, player, spec);
        }
        else
        {
            equipItem(set.GetFinger1_h(), SLOT_FINGER1, player, spec);
        }

        if (player->GetTeamId(true) == TEAM_ALLIANCE || set.GetFinger2_h() == 0)
        {
            equipItem(set.GetFinger2(), SLOT_FINGER2, player, spec);
        }
        else
        {
            equipItem(set.GetFinger2_h(), SLOT_FINGER2, player, spec);
        }
        
        if (player->GetTeamId(true) == TEAM_ALLIANCE || set.GetTrinket1_h() == 0)
        {
            equipItem(set.GetTrinket1(), SLOT_TRINKET1, player, spec);
        }
        else
        {
            equipItem(set.GetTrinket1_h(), SLOT_TRINKET1, player, spec);
        }
        
        if (player->GetTeamId(true) == TEAM_ALLIANCE || set.GetTrinket2_h() == 0)
        {
            equipItem(set.GetTrinket2(), SLOT_TRINKET2, player, spec);
        }
        else
        {
            equipItem(set.GetTrinket2_h(), SLOT_TRINKET2, player, spec);
        }
        
        //player->InitStatsForLevel(true);

        player->SetHealth(player->GetMaxHealth());
        if (player->getPowerType() == POWER_MANA || player->getClass() == CLASS_DRUID)
        {
            player->SetPower(POWER_MANA, player->GetMaxPower(POWER_MANA));
        }
        
        return true;
    }

    void setEnchantAndSocket(Player* player, Item* item, uint32 entry, uint32 spec)
    {
        uint32 id = (entry * 10000) + (player->getClass()*100) + spec;

        map<uint64, AzthGearScalingSocket> & AllItemsSockets = sAzthGearScalingSocketMgr->GetGearScalingSocketList();
        map<uint64, AzthGearScalingSocket>::iterator iter = AllItemsSockets.find(id);
        if (iter != AllItemsSockets.end()) //items has at least 1 socket or 1 enchant
        {
            AzthGearScalingSocket socketList = AllItemsSockets[id];


            std::vector<uint32> gems;
            gems.push_back(socketList.GetSocket1());
            gems.push_back(socketList.GetSocket2());
            gems.push_back(socketList.GetSocket3());

            //add socket to item
            for (uint32 i = 0; i < gems.size(); i++)
            {
                if (gems[i] != 0)
                {
                    ItemTemplate const* _proto = sObjectMgr->GetItemTemplate(gems[i]);
                    GemPropertiesEntry const* gemProperty = sGemPropertiesStore.LookupEntry(_proto->GemProperties);
                    item->SetEnchantment((EnchantmentSlot)(SOCK_ENCHANTMENT_SLOT + i), gemProperty->spellitemenchantement, 0, 0, player->GetGUID());
                    player->ApplyEnchantment(item, (EnchantmentSlot)(SOCK_ENCHANTMENT_SLOT + i), true);
                }
            }

            //set enchant to item
            if (socketList.GetEnchant() != 0)
            {
                SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(socketList.GetEnchant());
                for (uint32 i = 0; i < 3; i++)
                {
                    uint32 enchant_id = spellInfo->Effects[i].MiscValue;
                    if (!enchant_id)
                        continue;

                    SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
                    if (!pEnchant)
                        continue;

                    item->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchant_id, 0, 0, player->GetGUID());
                    player->ApplyEnchantment(item, PERM_ENCHANTMENT_SLOT, true);
                }
            }
        }
        
        // if we want immediately save
        //SQLTransaction trans = CharacterDatabase.BeginTransaction();
        //item->SaveToDB(trans);
        //CharacterDatabase.CommitTransaction(trans);
    }
};

class tournamentPlayer : public PlayerScript
{
public:
    tournamentPlayer() : PlayerScript("tournamentPlayer") {}
    
    void OnLoadFromDB(Player* player) override {
        // load pvp set if any
        QueryResult PVPSetCharactersActive_table = CharacterDatabase.PQuery(("SELECT id,season,spec FROM azth_tournamentset_active WHERE id = %d;"), player->GetGUID());

        if (PVPSetCharactersActive_table)
            player->azthPlayer->SetTempGear(true);
    }

    void OnUpdateZone(Player* player, uint32  newZone, uint32 /*newArea*/) override
    {
        if (player->GetSession()->PlayerLoading())
            return; // do not remove set during login
            
        if (player->azthPlayer->isPvP())
            return; // do not remove set for pvp players
            
        uint32 curDimension=player->azthPlayer->getCurrentDimensionByAura();
        if (curDimension && (curDimension == DIMENSION_PVP || curDimension == DIMENSION_ENTERTAINMENT || curDimension == DIMENSION_TEST))
            return; // do not remove set on special dimensions
        
        uint32 zone;
        uint32 area;
        player->GetBaseMap()->GetZoneAndAreaId(zone, area, player->GetEntryPoint().GetPositionX(), player->GetEntryPoint().GetPositionY(), player->GetEntryPoint().GetPositionZ());
        if (newZone == zone)
            return; // do not remove set if we're leaving BG/Arena
        
        
        if (!player->IsGameMaster() && !player->InBattleground() && !player->InArena())
        {
            if (player->azthPlayer->hasGear())
            {
                for (uint32 INVENTORY_INDEX = EQUIPMENT_SLOT_START; INVENTORY_INDEX < EQUIPMENT_SLOT_END; INVENTORY_INDEX++)
                {
                    Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, INVENTORY_INDEX);
                    if (item != nullptr)
                    {
                        player->DestroyItem(INVENTORY_SLOT_BAG_0, INVENTORY_INDEX, true);
                    }
                }
                player->azthPlayer->SetTempGear(false);
                QueryResult PVPSetCharactersActive_table = CharacterDatabase.PQuery(("DELETE FROM azth_tournamentset_active WHERE  `id`=%d;"), player->GetGUID());
                player->SaveToDB(false, false);
                ChatHandler(player->GetSession()).PSendSysMessage("Il tuo set PVP e' stato rimosso, non puoi cambiare zona con un set temporaneo!");
                // teleport to dalaran
                player->TeleportTo(AzthSharedDef::blackMarket);
            }
        }
    }
};

void AddSC_TournamentSet()
{
    new loadTournamentSet();
    new tournamentGossip();
    new tournamentPlayer();
}
