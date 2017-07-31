#include "ScriptMgr.h"
#include "Player.h"
#include "LFG.h"
#include "AzthUtils.h"
#include "AzthPlayer.h"
#include "MapManager.h"

std::map<uint32, bool> playerIds;

class PvPMode : public GlobalScript
{
public:
    PvPMode() : GlobalScript("PvPMode") {}

    void OnInitializeLockedDungeons(Player* player, uint8& level, uint32& lockData)
    {
        if (player->azthPlayer->isPvP())
        {
            lockData = 6; // LFG_LOCKSTATUS_RAID_LOCKED; //disable raid/dungeon if is a character/pvp account
        }
    }

    void OnAfterInitializeLockedDungeons(Player* player)
    {
        if ((player->GetMap()->IsDungeon() || player->GetMap()->IsRaidOrHeroicDungeon()) && player->azthPlayer->isPvP())
        {
            player->TeleportTo(player->m_recallMap, player->m_recallX, player->m_recallY, player->m_recallZ, player->m_recallO);
            ChatHandler(player->GetSession()).PSendSysMessage("Stai usando un personaggio Full-PvP, non puoi partecipare a raid/dungeons.");
        }
    }
};

class PvPModeFirstLogin : public PlayerScript
{
public:
    PvPModeFirstLogin() : PlayerScript("PvPModeFirstLogin") {}
    
    void OnCreate(Player* player) override
    {
        player->azthPlayer->loadPvPInfo();
        
        if (player->azthPlayer->isPvP()) {
            player->setCinematic(1);
            player->SetLevel(80);

            WorldLocation startLoc;
            startLoc.m_positionX = 4718.45;
            startLoc.m_positionY = -1974.84;
            startLoc.m_positionZ = 1086.91;
            startLoc.m_orientation = 0.19;
            startLoc.m_mapId  = 1;

            player->Relocate(&startLoc);
            player->ResetMap();
            player->SetMap(sMapMgr->CreateMap(1, player));
            
            player->SaveToDB(false,false);
        }
    }

    void OnFirstLogin(Player* player) override
    {
        if (player->azthPlayer->isPvP())
        {
            playerIds[player->GetGUID()] = true;
        }
    }

    void OnLogin(Player* player) override
    {
        player->azthPlayer->loadPvPInfo();
        
        if (playerIds[player->GetGUID()])
        {
            // delevel + levelup to fix achievements 
            player->GiveLevel(79);
            player->GiveLevel(80); 

            player->InitTalentForLevel();
            player->learnDefaultSpells();
            player->SetUInt32Value(PLAYER_XP, 0);
            sAzthUtils->learnClassSpells(player, false);
            
            // riding + flying
            player->learnSpell(34091); // artisan riding
            player->learnSpell(54197); // coldweather flying
            
            player->learnSpell(41514); // flying mount, netherdrake
            player->learnSpell(54753); // ground mount, bear
            
            
            // explore all zones
            for (uint8 i = 0; i<PLAYER_EXPLORED_ZONES_SIZE; i++)
                player->SetFlag(PLAYER_EXPLORED_ZONES_1 + i, 0xFFFFFFFF);
            
            player->UpdateSkillsToMaxSkillsForLevel(); // set all skills
            
            player->StoreNewItemInBestSlots(23162, 8);
            
            for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
            {
                if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                {
                    uint16 eDest;
                    // equip offhand weapon/shield if it attempt equipped before main-hand weapon
                    InventoryResult msg = player->CanEquipItem(NULL_SLOT, eDest, pItem, false);
                    if (msg == EQUIP_ERR_OK)
                    {
                        player->RemoveItem(INVENTORY_SLOT_BAG_0, i, true);
                        player->EquipItem(eDest, pItem, true);
                    }
                    // move other items to more appropriate slots (ammo not equipped in special bag)
                    else
                    {
                        ItemPosCountVec sDest;
                        msg = player->CanStoreItem(NULL_BAG, NULL_SLOT, sDest, pItem, false);
                        if (msg == EQUIP_ERR_OK)
                        {
                            player->RemoveItem(INVENTORY_SLOT_BAG_0, i, true);
                            pItem = player->StoreItem(sDest, pItem, true);
                        }

                        // if  this is ammo then use it
                        msg = player->CanUseAmmo(pItem->GetEntry());
                        if (msg == EQUIP_ERR_OK)
                            player->SetAmmo(pItem->GetEntry());
                    }
                }
            }
            
            
            player->TeleportTo(1, 4718.45, -1974.84, 1086.91, 0.19); //teleport to black market
            playerIds.erase(player->GetGUID());
            player->SaveToDB(false, false);
        }
        
        if (player->azthPlayer->isPvP()){
            player->SetTaxiCheater(true); // must be set at each login
        }
    }


};


void AddSC_PvPMode()
{
    new PvPMode();
    new PvPModeFirstLogin();
}