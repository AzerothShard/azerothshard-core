#include "Apps.h"
#include "MapManager.h"

void SmartStoneApps::blackMarketTeleport(Player *player) {
    if (!player->IsInCombat() && !player->azthPlayer->isInBlackMarket()) {
        float mapid = player->GetMapId();
        std::vector<float> pos = {mapid, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ()};
        player->azthPlayer->setLastPositionInfo(pos);

        player->TeleportTo(1, 4818.27f, -1971.3f, 1069.75f, 0.174f, 0);
    }
    if (!player->IsInCombat() && player->azthPlayer->isInBlackMarket()) {
        std::vector<float> & pos = player->azthPlayer->getLastPositionInfo();

        Map *m = sMapMgr->FindBaseMap(pos[0]);

        if (m && m->IsDungeon()) {

            // pos[0] is the map
            WorldSafeLocsEntry const* ClosestGrave = sObjectMgr->GetClosestGraveyard(pos[1], pos[2], pos[3], pos[0], TEAM_NEUTRAL);

            if (ClosestGrave) {
                player->TeleportTo(ClosestGrave->map_id, ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, player->GetOrientation());
            }
        } else {
            player->TeleportTo(pos[0], pos[1], pos[2], pos[3], 0.174f, 0);
        }
    }   
}

void SmartStoneApps::jukebox(Player *player) {
    if (player->FindNearestCreature(300205, 20.f, true) != NULL)
        return;

    TempSummon* jukebox_summon = player->SummonCreature(300205, player->GetPositionX(),
            player->GetPositionY(), player->GetPositionZ(),
            player->GetOrientation(),
            TEMPSUMMON_TIMED_DESPAWN, 600 * 1000, 0);
    Map* playerMap = player->GetMap();
    Creature* jukebox = playerMap->GetCreature(jukebox_summon->GetGUID());
    jukebox->GetMotionMaster()->MoveFollow(player, 2, 2);   
}