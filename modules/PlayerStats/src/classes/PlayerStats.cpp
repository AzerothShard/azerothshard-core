#include "AzthPlayer.h"
#include "Group.h"
#include "Player.h"

uint8 AzthPlayer::getGroupLevel() {
  uint8 groupLevel = 0;
  
  if (!player)
      return groupLevel;

  Group *group = player->GetGroup();
  Map *map = player->FindMap();
  if (group) {
    if (map->IsDungeon()) {
      // caso party instance
      InstanceSave *is = sInstanceSaveMgr->PlayerGetInstanceSave(
          GUID_LOPART(player->GetGUID()), map->GetId(),
          player->GetDifficulty((map->IsRaid())));

      if (is != NULL) {
        groupLevel = is->azthInstMgr->levelMax;
        return groupLevel;
      }
    }

    // outworld party or limit case for dungeon
    groupLevel = group->azthGroupMgr->levelMaxGroup;
  }

  return groupLevel;
}