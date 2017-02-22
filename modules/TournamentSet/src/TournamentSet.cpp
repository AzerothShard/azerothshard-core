#include "ScriptMgr.h"
#include "Unit.h"
#include "SharedDefines.h"
#include "Player.h"
#include "Common.h"
#include "AzthPlayer.h"
#include "AzthGearScaling.h"
#include "AzthGearScalingSocket.h"

std::map<uint32, AzthGearScaling> tournamentTempGearList;
std::map<uint32, AzthGearScalingSocket> tournamentTempGearSocketList;

class loadTournamentSet : public WorldScript
{
public:
    loadTournamentSet() : WorldScript("loadTournamentSet") {}
    void OnStartup()
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

        sAzthGearScaling->SetGearScalingList(tournamentTempGearList);

        //-------------------------------------------------------------------------------

        QueryResult tournament_socketEnchants_table = ExtraDatabase.PQuery("SELECT itemId,socket1,socket2,socket3,enchant FROM tournament_socket_enchants");
        if (!tournament_socketEnchants_table)
        {
            sLog->outString(">> Loaded 0 tournament Gear Socket for Tournament Set. DB table `tournament_socket_enchants` is empty.\n");
        }

        else
        {
            Field* tournament_socketEnchants_Field = tournament_socketEnchants_table->Fetch();
            do
            {
                tournamentTempGearSocketList[tournament_socketEnchants_Field[0].GetUInt32()] = AzthGearScalingSocket(tournament_socketEnchants_Field[0].GetUInt32(), tournament_socketEnchants_Field[1].GetUInt32(), tournament_socketEnchants_Field[2].GetUInt32(), tournament_socketEnchants_Field[3].GetUInt32(), tournament_socketEnchants_Field[4].GetUInt32());
            } while (tournament_socketEnchants_table->NextRow());

            sAzthGearScalingSocket->SetGearScalingSocketList(tournamentTempGearSocketList);
        }
    }
       
};

class tournamentGossip : public CreatureScript
{
public:
    tournamentGossip() : CreatureScript("tournamentGossip") {}

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->getLevel()>=80) {
            if (!player->azthPlayer->hasGear())
            {
                string str = "Gli item attualmente equippati saranno mandati alla mail, vuoi proseguire?";
                player->ADD_GOSSIP_ITEM_EXTENDED(0, "Deadly", GOSSIP_SENDER_MAIN, 5, str, 0, false);
                player->ADD_GOSSIP_ITEM_EXTENDED(0, "Furious", GOSSIP_SENDER_MAIN, 6, str, 0, false);
                player->ADD_GOSSIP_ITEM_EXTENDED(0, "Relentless", GOSSIP_SENDER_MAIN, 7, str, 0, false);
                //player->ADD_GOSSIP_ITEM_EXTENDED(0, "Wrathful", GOSSIP_SENDER_MAIN, 8, str.c_str(), 0, false);
            }
            else
                player->ADD_GOSSIP_ITEM(0, "Rimuovi il set PVP", GOSSIP_SENDER_MAIN, 9);
        }

        player->SEND_GOSSIP_MENU(1, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        if (action >= 4 && action <= 8) // deadly = 5, furious = 6, relentless = 7, wrathful = 8 -> go to select spec menu
            selectSpec(player, creature, action);

        if (action >= 10000) //spec selected -> equip new gear
        {
            uint32 season = action / 10000;
            uint32 spec = action - (season * 10000) - (player->getClass()*100);
            AzthGearScaling set = sAzthGearScaling->GetGearScalingList()[action];
            equipSet(set, player);
            player->azthPlayer->SetTempGear(true);
            QueryResult PVPSetCharactersActive_table = CharacterDatabase.PQuery(("INSERT IGNORE INTO azth_tournamentset_active (`id`, `season`, `spec`) VALUES ('%d', '%d', '%d');"), player->GetGUID(), season, spec);
            player->SaveToDB(false, false);
            player->PlayerTalkClass->SendCloseGossip();
        }

        if (action == 9) //exit from this mode
        {
            uint32 INVENTORY_END = 18;
            for (uint32 INVENTORY_INDEX = 0; INVENTORY_INDEX <= INVENTORY_END; INVENTORY_INDEX++)
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
                player->ADD_GOSSIP_ITEM(0, it->second.GetName().c_str(), GOSSIP_SENDER_MAIN, season * 10000 + it->second.GetClass() * 100 + it->second.GetSpec());
            }
        }
        player->SEND_GOSSIP_MENU(1, creature->GetGUID());
    }

    void equipSet(AzthGearScaling set, Player* player)
    {
        uint32 INVENTORY_END = 18;

        //remove equipped items and send to mail
        SQLTransaction trans = CharacterDatabase.BeginTransaction();
        MailDraft* draft = new MailDraft("Item rimossi", "");
        bool hasItems=false;
        for (uint32 INVENTORY_INDEX = 0; INVENTORY_INDEX <= INVENTORY_END; INVENTORY_INDEX++)
        {
            Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, INVENTORY_INDEX);
            if (item != nullptr)
            {
                player->MoveItemFromInventory(INVENTORY_SLOT_BAG_0, INVENTORY_INDEX, true);
                item->DeleteFromInventoryDB(trans);                   // deletes item from character's inventory
                item->SaveToDB(trans);
                draft->AddItem(item);
                hasItems=true;
            }
        }
        if (hasItems)
            draft->SendMailTo(trans, player, MailSender(player, MAIL_STATIONERY_GM), MAIL_CHECK_MASK_COPIED);
        
        delete draft;

        CharacterDatabase.CommitTransaction(trans);


        if (set.GetHead() != 0)
        {
            Item* item = player->EquipNewItem(SLOT_HEAD, set.GetHead(), true);
            setEnchantAndSocket(player, item);
        }

        if (set.GetNeck() != 0)
        {
            Item* item = player->EquipNewItem(SLOT_NECK, set.GetNeck(), true);
            setEnchantAndSocket(player, item);
        }

        if (set.GetShoulders() != 0)
        {
            Item* item = player->EquipNewItem(SLOT_SHOULDERS, set.GetShoulders(), true);
            setEnchantAndSocket(player, item);
        }

        if (set.GetBody() != 0)
        {
            Item* item = player->EquipNewItem(EQUIPMENT_SLOT_BODY, set.GetBody(), true);
            setEnchantAndSocket(player, item);
        }

        if (set.GetChest() != 0)
        {
            Item* item = player->EquipNewItem(SLOT_CHEST, set.GetChest(), true);
            setEnchantAndSocket(player, item);
        }

        if (set.GetWaist() != 0)
        {
            Item* item = player->EquipNewItem(SLOT_WAIST, set.GetWaist(), true);
            setEnchantAndSocket(player, item);
        }

        if (set.GetLegs() != 0)
        {
            Item* item = player->EquipNewItem(SLOT_LEGS, set.GetLegs(), true);
            setEnchantAndSocket(player, item);
        }

        if (set.GetFeet() != 0)
        {
            Item* item = player->EquipNewItem(SLOT_FEET, set.GetFeet(), true);
            setEnchantAndSocket(player, item);
        }

        if (set.GetWrists() != 0) //ally or horde
        {
            if (set.GetWrists_h() != 0)
            {
                if (player->GetTeamId() == TEAM_ALLIANCE)
                {
                    Item* item = player->EquipNewItem(SLOT_WRISTS, set.GetWrists(), true);
                    setEnchantAndSocket(player, item);
                }
                else
                {
                    Item* item = player->EquipNewItem(SLOT_WRISTS, set.GetWrists_h(), true);
                    setEnchantAndSocket(player, item);
                }
            }
            else
            {
                Item* item = player->EquipNewItem(SLOT_WRISTS, set.GetWrists(), true);
                setEnchantAndSocket(player, item);
            }
        }

        if (set.GetHands() != 0)
        {
            Item* item = player->EquipNewItem(SLOT_HANDS, set.GetHands(), true);
            setEnchantAndSocket(player, item);
        }

        if (set.GetFinger1() != 0) //ally or horde
        {
            if (set.GetFinger1_h() != 0)
            {
                if (player->GetTeamId() == TEAM_ALLIANCE)
                {
                    Item* item = player->EquipNewItem(SLOT_FINGER1, set.GetFinger1(), true);
                    setEnchantAndSocket(player, item);
                }
                else
                {
                    Item* item = player->EquipNewItem(SLOT_FINGER1, set.GetFinger1_h(), true);
                    setEnchantAndSocket(player, item);
                }
            }
            else
            {
                Item* item = player->EquipNewItem(SLOT_FINGER1, set.GetFinger1(), true);
                setEnchantAndSocket(player, item);
            }
        }

        if (set.GetFinger2() != 0) //ally or horde
        {
            if (set.GetFinger2_h() != 0)
            {
                if (player->GetTeamId() == TEAM_ALLIANCE)
                {
                    Item* item = player->EquipNewItem(SLOT_FINGER2, set.GetFinger2(), true);
                    setEnchantAndSocket(player, item);
                }
                else
                {
                    Item* item = player->EquipNewItem(SLOT_FINGER2, set.GetFinger2_h(), true);
                    setEnchantAndSocket(player, item);
                }
            }
            else
            {
                Item* item = player->EquipNewItem(SLOT_FINGER2, set.GetFinger2(), true);
                setEnchantAndSocket(player, item);
            }
        }

        if (set.GetTrinket1() != 0) //ally or horde
        {
            if (set.GetTrinket1_h() != 0)
            {
                if (player->GetTeamId() == TEAM_ALLIANCE)
                {
                    Item* item = player->EquipNewItem(SLOT_TRINKET1, set.GetTrinket1(), true);
                    setEnchantAndSocket(player, item);
                }
                else
                {
                    Item* item = player->EquipNewItem(SLOT_TRINKET1, set.GetTrinket1_h(), true);
                    setEnchantAndSocket(player, item);
                }
            }
            else
            {
                Item* item = player->EquipNewItem(SLOT_TRINKET1, set.GetTrinket1(), true);
                setEnchantAndSocket(player, item);
            }
        }

        if (set.GetTrinket2() != 0) //ally or horde
        {
            if (set.GetTrinket2_h() != 0)
            {
                if (player->GetTeamId() == TEAM_ALLIANCE)
                {
                    Item* item = player->EquipNewItem(SLOT_TRINKET2, set.GetTrinket2(), true);
                    setEnchantAndSocket(player, item);
                }
                else
                {
                    Item* item = player->EquipNewItem(SLOT_TRINKET2, set.GetTrinket2_h(), true);
                    setEnchantAndSocket(player, item);
                }
            }
            else
            {
                Item* item = player->EquipNewItem(SLOT_TRINKET2, set.GetTrinket2(), true);
                setEnchantAndSocket(player, item);
            }
        }

        if (set.GetBack() != 0)
        {
            Item* item = player->EquipNewItem(SLOT_BACK, set.GetBack(), true);
            setEnchantAndSocket(player, item);
        }

        if (set.GetMainHand() != 0)
        {
            Item* item = player->EquipNewItem(SLOT_MAIN_HAND, set.GetMainHand(), true);
            setEnchantAndSocket(player, item);
        }

        if (set.GetOffHand() != 0)
        {
            Item* item = player->EquipNewItem(SLOT_OFF_HAND, set.GetOffHand(), true);
            setEnchantAndSocket(player, item);
        }

        if (set.GetRanged() != 0)
        {
            Item* item = player->EquipNewItem(SLOT_RANGED, set.GetRanged(), true);
            setEnchantAndSocket(player, item);
        }

        if (set.GetTabard() != 0)
        {
            Item* item = player->EquipNewItem(SLOT_TABARD, set.GetTabard(), true);
            setEnchantAndSocket(player, item);
        }
        
    }

    void setEnchantAndSocket(Player* player, Item* item)
    {
        item->GetEntry();
        map<uint32, AzthGearScalingSocket> AllItemsSockets = sAzthGearScalingSocket->GetGearScalingSocketList();
        map<uint32, AzthGearScalingSocket>::iterator iter = AllItemsSockets.find(item->GetEntry());
        if (iter == AllItemsSockets.end())
        {
            //items doesnt have socket nor enchant
            return;
        }

        AzthGearScalingSocket socketList = AllItemsSockets[item->GetEntry()];


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
                    return;

                SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
                if (!pEnchant)
                    return;

                item->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchant_id, 0, 0, player->GetGUID());
            }
        }
    }
};

class tournamentPlayer : public PlayerScript
{
public:
    tournamentPlayer() : PlayerScript("tournamentPlayer") {}

    void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea) override
    {
        if (newZone == newArea)
            return;

        if (!player->IsGameMaster() && (!player->InBattleground() || !player->InArena()))
        {
            if (player->azthPlayer->hasGear())
            {
                uint32 INVENTORY_END = 18;
                for (uint32 INVENTORY_INDEX = 0; INVENTORY_INDEX <= INVENTORY_END; INVENTORY_INDEX++)
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
                WorldLocation loc = WorldLocation(571, 5804.15f, 624.771f, 647.767f, 1.64f);
                player->TeleportTo(loc);
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
