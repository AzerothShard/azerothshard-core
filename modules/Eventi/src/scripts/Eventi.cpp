#include "Eventi.h"
// By Gargarensis with Love <3

#include "ScriptMgr.h"
#include "WorldPacket.h"
#include "Player.h"
#include "GameObject.h"
#include "ObjectMgr.h"
#include "ObjectAccessor.h"
#include "OutdoorPvPMgr.h"
#include "Creature.h"
#include "Language.h"
#include "World.h"
#include "GossipDef.h"
#include "Group.h"
#include "AzthPlayer.h"


class eventi_go_sweet_chest : public GameObjectScript
{
public:
    eventi_go_sweet_chest() : GameObjectScript("eventi_go_sweet_chest") { }

    std::map<uint64, std::vector<uint64>> lootedMap;

    bool OnGossipHello(Player* player, GameObject* pGO)
    {
        map<uint64, std::vector<uint64>>::iterator it = lootedMap.find(pGO->GetGUID());
        std::vector<uint64> alreadyLooted;

        if (it != lootedMap.end())
        {
            alreadyLooted = it->second;
        } 


        for (std::vector<uint64>::iterator itr = alreadyLooted.begin(); itr != alreadyLooted.end(); ++itr) 
        {
            if ((*itr) && (*itr) == player->GetGUID())
            {
                player->SendEquipError(EQUIP_ERR_ALREADY_LOOTED, NULL, NULL);
                return false;
            }
        }

        if (!player->GetGroup()) 
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Devi essere in un gruppo di due persone per aprire questa cassa!");
            return false;
        }

        if (player->GetGroup()->GetMembersCount() != 2)
        { 
            ChatHandler(player->GetSession()).PSendSysMessage("Devi essere in un gruppo di due persone per aprire questa cassa!");
            return false;
        }


        for (std::vector<uint64>::iterator itr = alreadyLooted.begin(); itr != alreadyLooted.end(); ++itr)
        {
            Player* temp = player->GetMap()->GetPlayer((*itr));

            if (!temp)
            {
                continue;
            }

            if (temp->IsInSameGroupWith(player))
            {
                switch (pGO->GetEntry()) 
                {
                    case 300802:
                        player->AddItem(37711, 3);
                        temp->AddItem(37711, 3);
                        break;
                    case 300803:
                        player->AddItem(37711, 6);
                        temp->AddItem(37711, 6);
                        break;
                    case 300804:
                        player->AddItem(37711, 10);
                        temp->AddItem(37711, 10);
                        break;
                    default:
                        player->AddItem(37711, 1);
                        temp->AddItem(37711, 1);
                        break;
                }
                break;
            }
        }

        alreadyLooted.push_back(player->GetGUID());
        lootedMap[pGO->GetGUID()] = alreadyLooted;

        return false;
    }
};


void AddEventiScripts()
{
    new eventi_go_sweet_chest();
}
