#include "AzthPlayer.h"
#include "AzthLevelStat.h"
#include "Group.h"
#include "Player.h"

uint32 AzthPlayer::getGroupLevel(bool normalized) {
  uint32 groupLevel = 0;
  
  if (!player)
      return groupLevel;

  Group *group = player->GetGroup();
  Map *map = player->FindMap();
  if (group) {
    if (map->IsDungeon() || map->IsRaid()) {
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
  
  if (normalized && groupLevel>=TIMEWALKING_SPECIAL_LVL_MIN)
      groupLevel = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);

  return groupLevel;
}