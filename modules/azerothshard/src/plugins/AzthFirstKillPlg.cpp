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
#include "AzthLevelStat.h"
#include "AzthFirstKills.h"
#include "azth_custom_hearthstone_mode.h"


class achievement_first_kills_script : public AchievementCriteriaScript
{
    public:
        achievement_first_kills_script(char const* name, uint32 achiId) : AchievementCriteriaScript(name), _achiId(achiId)
        {
        }

        bool OnCheck(Player*  player, Unit* /*target*/) override
        {
            if (player->azthPlayer->isTimeWalking()) {
                uint32 plLevel=player->azthPlayer->GetTimeWalkingLevel();
                uint32 groupLevel=player->azthPlayer->getGroupLevel(false);
                
                uint32 level=groupLevel > plLevel ? groupLevel : plLevel;
                
                switch (_achiId) {
                    case ACHI_NAXXRAMAS:
                        return level == TIMEWALKING_LVL_NAXX;
                    case ACHI_OBSIDIAN:
                        return level == TIMEWALKING_LVL_OBSIDIAN;
                    case ACHI_MAGIC_SEEKER:
                        return level == TIMEWALKING_LVL_THE_EYE;
                    case ACHI_DEATH_DEMISE:
                    case ACHI_CELESTIAL_DEFENDER:
                        return level == TIMEWALKING_LVL_ULDUAR;
                    case ACHI_GRAND_CRUSADER:
                        return level == TIMEWALKING_LVL_TOGC;
                }
            }
            
            return false;
        }
    private:
        uint32 _achiId;
};


void AddSC_first_kill_achievements() {
    new achievement_first_kills_script("achievement_first_kill_naxxramas", ACHI_NAXXRAMAS);
    new achievement_first_kills_script("achievement_first_kill_obsidian", ACHI_OBSIDIAN);
    new achievement_first_kills_script("achievement_first_kill_magic_seeker", ACHI_MAGIC_SEEKER);
    new achievement_first_kills_script("achievement_first_kill_celestial_defender", ACHI_CELESTIAL_DEFENDER);
    new achievement_first_kills_script("achievement_first_kill_grand_crusader", ACHI_GRAND_CRUSADER);
}