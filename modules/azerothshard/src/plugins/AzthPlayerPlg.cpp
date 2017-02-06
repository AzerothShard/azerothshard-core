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

enum achievementStatsType {
    ACHIEVEMENT_TYPE,
    CRITERIA_TYPE
};

class AzthPlayerPlg : public PlayerScript{
public:

    AzthPlayerPlg() : PlayerScript("AzthPlayerPlg") { }
    
    void OnLevelChanged(Player* player, uint8 oldLevel) override
    {
        if (oldLevel == 9)
        {
            sWorld->SendGameMail(player, "Well done!", "You reached level 10, a small present for you!", 10 * GOLD);
        }
    }

    void OnLogin(Player* player) override {
        // do it again for crossfaction system
        player->setFactionForRace(player->getRace());

        //set last known position to black market
        std::vector<float> pos = player->azthPlayer->getLastPositionInfoFromDB();
        player->azthPlayer->setLastPositionInfo(pos);
    }

    void OnLogout(Player* player) override {
        player->azthPlayer->saveLastPositionInfoToDB(player->azthPlayer->getLastPositionInfo());
    }

    void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea) override {

        Map* map = player->FindMap();
        uint32 mapId = map->GetId();

        if (mapId == 0
        || mapId == 1
        || mapId == 530
        || mapId == 571)
        {
            player->setFactionForRace(player->getRace());
        }

        uint16 levelPlayer = player->getLevel();

        if (map->IsDungeon()) {
            InstanceSave* is = sInstanceSaveMgr->PlayerGetInstanceSave(GUID_LOPART(player->GetGUID()), map->GetId(), player->GetDifficulty(map->IsRaid()));
            if (is->azthInstMgr->levelMax == 0) {
                player->azthPlayer->instanceID = map->GetInstanceId();

                QueryResult result = CharacterDatabase.PQuery("SELECT levelPg FROM instance WHERE id = %u", player->azthPlayer->instanceID);
                if (!result)
                    return;
                Field* fields = result->Fetch();
                is->azthInstMgr->levelMax = fields[0].GetUInt32();
            }

            if (levelPlayer > is->azthInstMgr->levelMax) {
                is->azthInstMgr->levelMax = levelPlayer;
                is->InsertToDB();
            }
        }
    }

    // Following 2 functions store levels in a temporary map
    void OnAchiComplete(Player *player, AchievementEntry const* achievement) override {
        AzthPlayer::AzthAchiData& it = player->azthPlayer->m_completed_achievement_map[achievement->ID];
        it.level = player->getLevel();
        it.levelParty = player->azthPlayer->getGroupLevel();
    }

    void OnCriteriaProgress(Player *player, AchievementCriteriaEntry const* criteria) override {
        AzthPlayer::AzthAchiData& it = player->azthPlayer->m_completed_criteria_map[criteria->ID];
        it.level = player->getLevel();
        it.levelParty = player->azthPlayer->getGroupLevel();
    }

    // Following 2 functions save our temporary maps inside the db
    void OnAchiSave(SQLTransaction& trans, Player *player, uint16 achId, CompletedAchievementData achiData) override {

        AzthPlayer::AzthAchiData& it = player->azthPlayer->m_completed_achievement_map[achId];
        uint32 index = 0;
        
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PVESTATS);
        stmt->setUInt32(index++, player->GetGUID());
        stmt->setUInt32(index++, achId);
        stmt->setUInt32(index++, ACHIEVEMENT_TYPE);
        stmt->setUInt32(index++, it.level);
        stmt->setUInt32(index++, it.levelParty);
        stmt->setUInt32(index++, achiData.date);
        trans->Append(stmt);

        player->azthPlayer->m_completed_achievement_map.erase(achId);
    }

    void OnCriteriaSave(SQLTransaction& trans, Player* player, uint16 critId, CriteriaProgress criteriaData) override {

        AzthPlayer::AzthAchiData& it = player->azthPlayer->m_completed_criteria_map[critId];
        uint32 index = 0;

        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PVESTATS);
        stmt->setUInt32(index++, player->GetGUID());
        stmt->setUInt32(index++, critId);
        stmt->setUInt32(index++, CRITERIA_TYPE);
        stmt->setUInt32(index++, it.level);
        stmt->setUInt32(index++, it.levelParty);
        stmt->setUInt32(index++, criteriaData.date);
        trans->Append(stmt);

        player->azthPlayer->m_completed_criteria_map.erase(critId);
    }

    void OnAfterStoreOrEquipNewItem(Player* player, uint32 vendorslot, uint32 &item, uint8 count, uint8 bag, uint8 slot, ItemTemplate const* pProto, Creature* pVendor, VendorItem const* crItem, bool bStore) override
    {
        long price = 0;

        VendorItemData const* vItems = pVendor->GetVendorItems();
        if (!vItems || vItems->Empty())
        {
            return;
        }

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

