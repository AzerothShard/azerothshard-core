#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "InstanceSaveMgr.h"
#include "Player.h"
#include "Map.h"
#include "WorldSession.h"
#include "Group.h"
#include "AzthUtils.h"

enum achievementStatsType {
    ACHIEVEMENT_TYPE,
    CRITERIA_TYPE
};

class AzthGroupPlg : public GroupScript {
public:

    AzthGroupPlg() : GroupScript("AzthGroupPlg") {
    }

    void OnAddMember(Group* group, uint64 guid) override {
        Player* player = ObjectAccessor::FindPlayer(guid);
        
        sAzthUtils->updateTwLevel(player, group);
    }
};

class PlayerStats : public PlayerScript {
public:

    PlayerStats() : PlayerScript("PlayerStats") {
    }

    void OnUpdateZone(Player* /*player*/, uint32  /*newZone*/, uint32  /*newArea*/) override {
        // sAzthUtils->updateTwLevel(player, player->GetGroup()); <-- we will do it in TimeWalking.cpp script, having correct order with autoscaling
    }


    // Following 2 functions store levels in a temporary map

    void OnAchiComplete(Player *player, AchievementEntry const* achievement) override {
        AzthPlayer::AzthAchiData it = {
            player->getLevel(),
            player->azthPlayer->getGroupLevel(),
            player->azthPlayer->getPStatsLevel(false)
        };
        
        player->azthPlayer->m_completed_achievement_map[achievement->ID] = it;
    }

    void OnCriteriaProgress(Player *player, AchievementCriteriaEntry const* criteria) override {
        AzthPlayer::AzthAchiData it = {
            player->getLevel(),
            player->azthPlayer->getGroupLevel(),
            player->azthPlayer->getPStatsLevel(false)
        };
        
        player->azthPlayer->m_completed_criteria_map[criteria->ID] = it;
    }

    // Following 2 functions save our temporary maps inside the db

    void OnAchiSave(SQLTransaction& trans, Player *player, uint16 achId, CompletedAchievementData achiData) override {
        if (player->azthPlayer->m_completed_achievement_map.find(achId) != player->azthPlayer->m_completed_achievement_map.end()) {
            AzthPlayer::AzthAchiData it = player->azthPlayer->m_completed_achievement_map[achId];
            uint32 index = 0;

            PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PVESTATS);
            stmt->setUInt32(index++, player->GetGUID());
            stmt->setUInt32(index++, achId);
            stmt->setUInt32(index++, ACHIEVEMENT_TYPE);
            stmt->setUInt32(index++, it.level);
            stmt->setUInt32(index++, it.levelParty);
            stmt->setUInt32(index++, it.specialLevel);
            stmt->setUInt32(index++, achiData.date);
            trans->Append(stmt);

            player->azthPlayer->m_completed_achievement_map.erase(achId); 
        }
    }

    void OnCriteriaSave(SQLTransaction& trans, Player* player, uint16 critId, CriteriaProgress criteriaData) override {
        if (player->azthPlayer->m_completed_criteria_map.find(critId) != player->azthPlayer->m_completed_criteria_map.end()) {
            AzthPlayer::AzthAchiData it = player->azthPlayer->m_completed_criteria_map[critId];
            uint32 index = 0;

            PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PVESTATS);
            stmt->setUInt32(index++, player->GetGUID());
            stmt->setUInt32(index++, critId);
            stmt->setUInt32(index++, CRITERIA_TYPE);
            stmt->setUInt32(index++, it.level);
            stmt->setUInt32(index++, it.levelParty);
            stmt->setUInt32(index++, it.specialLevel);
            stmt->setUInt32(index++, criteriaData.date);
            trans->Append(stmt);

            player->azthPlayer->m_completed_criteria_map.erase(critId);
        }
    }

};

void AddSC_playerstats() {
    new AzthGroupPlg();
    new PlayerStats();
}
