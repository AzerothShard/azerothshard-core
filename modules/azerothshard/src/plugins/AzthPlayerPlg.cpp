/**
    This plugin can be used for common player customizations
 */

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "InstanceSaveMgr.h"
#include "Player.h"
#include "Map.h"
#include "WorldSession.h"
#include "AchievementMgr.h"
#include "AzthGroupMgr.h"
#include "AzthPlayer.h"
#include "azth_custom_hearthstone_mode.h"

class AzthPlayerPlg : public PlayerScript{
public:

    AzthPlayerPlg() : PlayerScript("AzthPlayerPlg") { }
    
    void OnLevelChanged(Player* player, uint8 oldLevel) override
    {
        if (oldLevel == 9 && player->azthPlayer->GetTimeWalkingLevel() == 0)
        {
            sWorld->SendGameMail(player, "Well done!", "You reached level 10, a small present for you!", 10 * GOLD);
        }
    }

    // logger for custom extended costs
    void OnAfterStoreOrEquipNewItem(Player* player, uint32 vendorslot, uint32 &item, uint8 count, uint8 bag, uint8 slot, ItemTemplate const* pProto, Creature* pVendor, VendorItem const* crItem, bool bStore) override
    {
        long price = 0;

        if (crItem->ExtendedCost)
        {
            if (crItem->ExtendedCost <= 2997)
                return;

            price = crItem->ExtendedCost;
            price = price * (-1);
        }
        else
        {
            /*ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(item);
            if (!pProto)
            {
                return;
            }

            if (crItem->IsGoldRequired(pProto))
            {
                price = pProto->BuyPrice;
            }*/
            return;
        }

        uint32 guid = pVendor ? pVendor->GetEntry() : 0;

        ExtraDatabase.PQuery("INSERT INTO `buy_log` (`playerGuid`, `item`, `vendor`, `price`) VALUES (%u, %u, %u, %ld);",
            player->GetGUIDLow(), item, guid, price);
    }

};



void AddSC_azth_player_plg() {
    new AzthPlayerPlg();
}

