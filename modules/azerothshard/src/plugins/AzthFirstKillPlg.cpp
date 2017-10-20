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


class achievement_first_kill_obsidian : public AchievementCriteriaScript
{
    public:
        achievement_first_kill_obsidian(char const* name) : AchievementCriteriaScript(name)
        {
        }

        bool OnCheck(Player*  player, Unit* target /*Flame Leviathan*/)
        {
            /*Map::PlayerList const &pl = me->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator itr = pl.begin(); itr != pl.end(); ++itr)
                if (Player* plr = itr->GetSource())
                    if (!plr->IsGameMaster() && plr->IsInCombat())
                    {
                        for (uint8 i=EQUIPMENT_SLOT_START; i<EQUIPMENT_SLOT_END; ++i) // loop through equipped items
                            if (Item* item = plr->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                                if (!IsValidHeraldItem(item->GetTemplate()))
                                {
                                    _heraldOfTheTitans = false;
                                    return true;
                                }
                    }*/
            
            return player->azthPlayer->GetTimeWalkingLevel() > 0;
        }
};


void AddSC_first_kill_achievements() {
    new achievement_first_kill_obsidian("achievement_first_kill_obsidian");
}