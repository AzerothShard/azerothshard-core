#include "AzthPlayer.h"
#include "Player.h"
#include "Group.h"

// Send a kill credit, skipping the normal checks on raid group.
void AzthPlayer::ForceKilledMonsterCredit(uint32 entry, uint64 guid) {
  uint16 addkillcount = 1;
  uint32 real_entry = entry;
  if (guid) {
    Creature *killed = player->GetMap()->GetCreature(guid);
    if (killed && killed->GetEntry())
      real_entry = killed->GetEntry();
  }

  player->StartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_CREATURE,
                                real_entry); // MUST BE CALLED FIRST
  player->UpdateAchievementCriteria(
      ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, real_entry, addkillcount,
      guid ? player->GetMap()->GetCreature(guid) : NULL);

  for (uint8 i = 0; i < MAX_QUEST_LOG_SIZE; ++i) {
    uint32 questid = player->GetQuestSlotQuestId(i);
    if (!questid)
      continue;

    Quest const *qInfo = sObjectMgr->GetQuestTemplate(questid);
    if (!qInfo)
      continue;

    QuestStatusData &q_status = player->m_QuestStatus[questid];
    if (q_status.Status == QUEST_STATUS_INCOMPLETE && (qInfo->IsPVPQuest() && (player->GetGroup()->isBFGroup() || player->GetGroup()->isBGGroup()))) {
      if (qInfo->HasSpecialFlag(QUEST_SPECIAL_FLAGS_KILL) /*&& !qInfo->HasSpecialFlag(QUEST_SPECIAL_FLAGS_CAST)*/) {
        for (uint8 j = 0; j < QUEST_OBJECTIVES_COUNT; ++j) {
          // skip GO activate objective or none
          if (qInfo->RequiredNpcOrGo[j] <= 0)
            continue;

          uint32 reqkill = qInfo->RequiredNpcOrGo[j];

          if (reqkill == real_entry) {
            uint32 reqkillcount = qInfo->RequiredNpcOrGoCount[j];
            uint16 curkillcount = q_status.CreatureOrGOCount[j];
            if (curkillcount < reqkillcount) {
              q_status.CreatureOrGOCount[j] = curkillcount + addkillcount;

              player->m_QuestStatusSave[questid] = true;

              player->SendQuestUpdateAddCreatureOrGo(
                  qInfo, guid, j, curkillcount, addkillcount);
            }
            if (player->CanCompleteQuest(questid))
              player->CompleteQuest(questid);
            else
              player->AdditionalSavingAddMask(ADDITIONAL_SAVING_QUEST_STATUS);

            // same objective target can be in many active quests, but not in 2
            // objectives for single quest (code optimization).
            break;
          }
        }
      }
    }
  }
}