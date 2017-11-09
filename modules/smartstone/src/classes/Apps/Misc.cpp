#include "Apps.h"
#include "MapManager.h"
#include "AzthSharedDefines.h"

void SmartStoneApps::blackMarketTeleport(Player *player) {
    if (player->IsInCombat())
        return;
    
    if (!player->azthPlayer->isInBlackMarket()) {
        WorldLocation pos = WorldLocation(player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());
        player->azthPlayer->setLastPositionInfo(player->azthPlayer->getCurrentDimensionByAura(), pos);

        player->TeleportTo(AzthSharedDef::blackMarket);
    }
    else {
        WorldLocation const& pos = player->azthPlayer->getLastPositionInfo(player->azthPlayer->getCurrentDimensionByAura());
        if (!MapManager::IsValidMapCoord(pos))
            return;

        Map *m = sMapMgr->FindBaseMap(pos.GetMapId());

        if (m && m->IsDungeon()) {

            WorldSafeLocsEntry const* ClosestGrave = sObjectMgr->GetClosestGraveyard(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetMapId(), TEAM_NEUTRAL);

            if (ClosestGrave) {
                player->TeleportTo(ClosestGrave->map_id, ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, player->GetOrientation());
            }
        } else {
            player->TeleportTo(pos);
        }
    }   
}

void SmartStoneApps::teleportHouse(Player *owner, Player *guest) {
    if (guest->IsInCombat())
        return;

    WorldLocation const& pos = owner->azthPlayer->getLastPositionInfo(AZTH_SMRTST_POSITION_HOUSE_INDEX);
    if (!MapManager::IsValidMapCoord(pos))
        return;

    guest->TeleportTo(pos);
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