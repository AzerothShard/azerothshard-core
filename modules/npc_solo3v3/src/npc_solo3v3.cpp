
#include "ArenaTeamMgr.h"
#include "Common.h"
#include "DisableMgr.h"
#include "BattlegroundMgr.h"
#include "Battleground.h"
#include "ArenaTeam.h"
#include "Language.h"
#include "npc_solo3v3.h"
#include "BattlegroundQueue.h"


class npc_solo3v3 : public CreatureScript
{
public:
	npc_solo3v3() : CreatureScript("npc_solo3v3")
	{
		for (int i = 0; i < MAX_TALENT_CAT; i++)
			cache3v3Queue[i] = 0;

		lastFetchQueueList = 0;
	}

	bool OnGossipHello(Player* player, Creature* me)
	{
		if (!player || !me)
			return true;

        if (sConfigMgr->GetBoolDefault("Solo.3v3.Enable", true) == false)
		{
			ChatHandler(player->GetSession()).SendSysMessage("Arena disabled!");
			return true;
		}

		fetchQueueList();
		std::stringstream infoQueue;
		infoQueue << "Solo 3vs3 Arena\n";
		infoQueue << "Queued Players: " << (cache3v3Queue[MELEE] + cache3v3Queue[RANGE] + cache3v3Queue[HEALER]);

		if (sConfigMgr->GetBoolDefault("Solo.3v3.FilterTalents", false))
		{
			infoQueue << "\n\n";
			infoQueue << "Queued Melees: " << cache3v3Queue[MELEE] << " (Longer Queues!)" << "\n";
			infoQueue << "Queued Casters: " << cache3v3Queue[RANGE] << " (Longer Queues!)" << "\n";
			infoQueue << "Queued Healers: " << cache3v3Queue[HEALER] << " (Bonus Rewards!)" << "\n";
		}

		if (player->InBattlegroundQueueForBattlegroundQueueType(BATTLEGROUND_QUEUE_3v3_SOLO))
			player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "|TInterface/ICONS/Achievement_Arena_2v2_7:30|t Leave Solo queue", GOSSIP_SENDER_MAIN, 3, "Are you sure you want to remove the solo queue?", 0, false);

		if (player->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_SOLO_3v3)) == NULL)
			player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "|TInterface/ICONS/Achievement_Arena_2v2_7:30|t  Create new Solo arena team", GOSSIP_SENDER_MAIN, 1, "Create new solo arena team?", sConfigMgr->GetIntDefault("Solo.3v3.Cost", 1), false);
		else
		{
			if (player->InBattlegroundQueueForBattlegroundQueueType(BATTLEGROUND_QUEUE_3v3_SOLO) == false)
			{
			    //player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Queue up for 1vs1 Wargame\n", GOSSIP_SENDER_MAIN, 20);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "|TInterface/ICONS/Achievement_Arena_3v3_5:30|t Queue up for 3vs3 Arena Solo\n", GOSSIP_SENDER_MAIN, 2);
                player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Achievement_Arena_2v2_7:30|t Disband Arena team", GOSSIP_SENDER_MAIN, 5, "Are you sure?", 0, false);

			}

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/INV_Misc_Coin_01:30|t Show statistics", GOSSIP_SENDER_MAIN, 4);
		}

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/INV_Misc_Coin_03:30|t How to Use NPC?", GOSSIP_SENDER_MAIN, 8);

		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, infoQueue.str().c_str(), GOSSIP_SENDER_MAIN, 0);
		player->SEND_GOSSIP_MENU(60015, me->GetGUID());
		return true;
	}



	bool OnGossipSelect(Player* player, Creature* me, uint32 /*uiSender*/, uint32 uiAction)
	{
		if (!player || !me)
			return true;

		player->PlayerTalkClass->ClearMenus();

		switch (uiAction)
		{
		case 1: // Create new Arenateam
		{
			if (sConfigMgr->GetIntDefault("Solo.3v3.MinLevel", 80) <= player->getLevel())
			{
				if (player->GetMoney() >= sConfigMgr->GetIntDefault("Solo.3v3.Cost", 1) && CreateArenateam(player, me))
					player->ModifyMoney(sConfigMgr->GetIntDefault("Solo.3v3.Cost", 1) * -1);
			}
			else
			{
				ChatHandler(player->GetSession()).PSendSysMessage("You need level %u+ to create an arena team.", sConfigMgr->GetIntDefault("Solo.3v3.MinLevel", 80));
				player->CLOSE_GOSSIP_MENU();
				return true;
			}
		}
		break;

		case 2: // 3v3 Join Queue Arena (rated)
		{
			// check Deserter debuff
			if (player->HasAura(26013) && (sConfigMgr->GetBoolDefault("Solo.3v3.CastDeserterOnAfk", true) || sConfigMgr->GetBoolDefault("Solo.3v3.CastDeserterOnLeave", true)))
			{
				WorldPacket data;
				sBattlegroundMgr->BuildGroupJoinedBattlegroundPacket(&data, ERR_GROUP_JOIN_BATTLEGROUND_DESERTERS);
				player->GetSession()->SendPacket(&data);
			}
			else
				if (ArenaCheckFullEquipAndTalents(player) && JoinQueueArena(player, me, true, ARENA_TYPE_3v3_SOLO) == false)
					ChatHandler(player->GetSession()).SendSysMessage("Something went wrong while joining queue. Already in another queue?");

			player->CLOSE_GOSSIP_MENU();
			return true;
		}

		case 3: // Leave Queue
		{
            if (player->InBattlegroundQueueForBattlegroundQueueType(BATTLEGROUND_QUEUE_3v3_SOLO)) {
                uint8 arenaType = ARENA_TYPE_3v3_SOLO;

                WorldPacket Data;
                Data << arenaType << (uint8)0x0 << (uint32)BATTLEGROUND_AA << (uint16)0x0 << (uint8)0x0;
                player->GetSession()->HandleBattleFieldPortOpcode(Data);
                player->CLOSE_GOSSIP_MENU();

            }
            return true;
		}

        case 4: // get statistics
        {
            ArenaTeam* at = sArenaTeamMgr->GetArenaTeamById(player->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_SOLO_3v3)));
            if (at) {
                std::stringstream s;
                s << "Rating: " << at->GetStats().Rating;
                s << "\nRank: " << at->GetStats().Rank;
                s << "\nSeason Games: " << at->GetStats().SeasonGames;
                s << "\nSeason Wins: " << at->GetStats().SeasonWins;
                s << "\nWeek Games: " << at->GetStats().WeekGames;
                s << "\nWeek Wins: " << at->GetStats().WeekWins;

                ChatHandler(player->GetSession()).PSendSysMessage(s.str().c_str());
            }
            return true;
        }
        case 5: // Disband arenateam
        {
            WorldPacket Data;
            Data << (uint32)player->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_SOLO_3v3));
            player->GetSession()->HandleArenaTeamLeaveOpcode(Data);
            ChatHandler(player->GetSession()).PSendSysMessage("Arena team deleted!");
            player->CLOSE_GOSSIP_MENU();
            return true;
        }
        break;

        case 8: // Script Info
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Click on Create new 3v3 SoloQ Arena team", GOSSIP_SENDER_MAIN, uiAction);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Join 3v3 SoloQ Arena and ready!", GOSSIP_SENDER_MAIN, uiAction);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Enjoy!", GOSSIP_SENDER_MAIN, uiAction);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Back", GOSSIP_SENDER_MAIN, 7);
            player->SEND_GOSSIP_MENU(68, me->GetGUID());
            return true;
        }
        break;
		}

		OnGossipHello(player, me);
		return true;
	}

private:
	int cache3v3Queue[MAX_TALENT_CAT];
	uint32 lastFetchQueueList;

	bool ArenaCheckFullEquipAndTalents(Player* player)
	{
		if (!player)
			return false;

		if (!sConfigMgr->GetBoolDefault("Arena.CheckEquipAndTalents", true))
			return true;

		std::stringstream err;

		if (player->GetFreeTalentPoints() > 0)
			err << "You have currently " << player->GetFreeTalentPoints() << " free talent points. Please spend all your talent points before queueing arena.\n";

		Item* newItem = NULL;
		for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
		{
			if (slot == EQUIPMENT_SLOT_OFFHAND || slot == EQUIPMENT_SLOT_RANGED || slot == EQUIPMENT_SLOT_TABARD || slot == EQUIPMENT_SLOT_BODY)
				continue;

			newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
			if (newItem == NULL)
			{
				err << "Your character is not fully equipped.\n";
				break;
			}
		}

		if (err.str().length() > 0)
		{
			ChatHandler(player->GetSession()).SendSysMessage(err.str().c_str());
			return false;
		}
		return true;
	}

	bool JoinQueueArena(Player* player, Creature* me, bool isRated, uint8 arenatype)
	{
		if (!player || !me)
			return false;

		if (sConfigMgr->GetIntDefault("Solo.3v3.MinLevel",80) > player->getLevel())
			return false;

		uint64 guid = player->GetGUID();
		uint8 arenaslot = ArenaTeam::GetSlotByType(ARENA_TEAM_SOLO_3v3);
		uint32 arenaRating = 0;
		uint32 matchmakerRating = 0;

		// ignore if we already in BG or BG queue
		if (player->InBattleground() || player->InBattlegroundQueue())
			return false;

		//check existance
		Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(BATTLEGROUND_AA);
		if (!bg)
		{
			sLog->outError("Arena", "Battleground: template bg (all arenas) not found");
			return false;
		};

		if (DisableMgr::IsDisabledFor(DISABLE_TYPE_BATTLEGROUND, BATTLEGROUND_AA, NULL))
		{
			ChatHandler(player->GetSession()).PSendSysMessage(LANG_ARENA_DISABLED);
			return false;
		}

		BattlegroundTypeId bgTypeId = bg->GetBgTypeID();
		BattlegroundQueueTypeId bgQueueTypeId = BattlegroundMgr::BGQueueTypeId(bgTypeId, arenatype);
		PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), player->getLevel());
		if (!bracketEntry)
			return false;

		GroupJoinBattlegroundResult err = ERR_GROUP_JOIN_BATTLEGROUND_FAIL;

		// check if already in queue
		if (player->GetBattlegroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES)
			//player is already in this queue
			return false;
		// check if has free queue slots
		if (!player->HasFreeBattlegroundQueueId())
			return false;

		uint32 ateamId = 0;

		if (isRated)
		{
            ateamId = player->GetArenaTeamId(arenaslot);
			ArenaTeam* at = sArenaTeamMgr->GetArenaTeamById(ateamId);
			if (!at)
			{
				player->GetSession()->SendNotInArenaTeamPacket(arenatype);
				return false;
			}

			// get the team rating for queueing
			arenaRating = at->GetRating();
			matchmakerRating = at->GetAverageMMR(NULL);
			// the arenateam id must match for everyone in the group

			if (arenaRating <= 0)
				arenaRating = 1;
		}

		BattlegroundQueue &bgQueue = sBattlegroundMgr->GetBattlegroundQueue(bgQueueTypeId);
		//BattlegroundQueue &bgQueue = sBattlegroundMgr->m_BattlegroundQueues[bgQueueTypeId];
		bg->SetRated(isRated);

		GroupQueueInfo* ginfo = bgQueue.AddGroup(player, NULL, bracketEntry, isRated, false, arenaRating, matchmakerRating, ateamId);
		uint32 avgTime = bgQueue.GetAverageQueueWaitTime(ginfo);
		uint32 queueSlot = player->AddBattlegroundQueueId(bgQueueTypeId);

		WorldPacket data;
		// send status packet (in queue)
		sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, queueSlot, STATUS_WAIT_QUEUE, avgTime, 0, arenatype, player->GetTeamId()); //??
		player->GetSession()->SendPacket(&data);

		sBattlegroundMgr->ScheduleArenaQueueUpdate(arenatype, bgQueueTypeId, bracketEntry->GetBracketId());

        sScriptMgr->OnPlayerJoinArena(player);

		return true;
	}

	bool CreateArenateam(Player* player, Creature* me)
	{
		if (!player || !me)
			return false;

		uint8 slot = ArenaTeam::GetSlotByType(ARENA_TEAM_SOLO_3v3);

		// Check if player is already in an arena team
		if (player->GetArenaTeamId(slot))
		{
			player->GetSession()->SendArenaTeamCommandResult(ERR_ARENA_TEAM_CREATE_S, player->GetName(), "", ERR_ALREADY_IN_ARENA_TEAM);
			return false;
		}

		// Teamname = playername
		// if team name exist, we have to choose another name (playername + number)
		int i = 1;
		std::stringstream teamName;
		//teamName << "3vs3 Solo - " << player->GetName();
		teamName << player->GetName();
		do
		{
			if (sArenaTeamMgr->GetArenaTeamByName(teamName.str()) != NULL) // teamname exist, so choose another name
			{
				teamName.str(std::string());
				teamName << player->GetName() << i++;
			}
			else
				break;
		} while (i < 100); // should never happen

		// Create arena team
		ArenaTeam* arenaTeam = new ArenaTeam();

		if (!arenaTeam->Create(player->GetGUID(), ARENA_TEAM_SOLO_3v3, teamName.str(), 4283124816, 45, 4294242303, 5, 4294705149))
		{
			delete arenaTeam;
			return false;
		}

		// Register arena team
		sArenaTeamMgr->AddArenaTeam(arenaTeam);

		ChatHandler(player->GetSession()).SendSysMessage("Arena team successful created!");

		return true;
	}

	void fetchQueueList()
	{
		if (GetMSTimeDiffToNow(lastFetchQueueList) < 1000)
			return; // prevent spamming
		lastFetchQueueList = getMSTime();

		BattlegroundQueue* queue = &sBattlegroundMgr->GetBattlegroundQueue(BATTLEGROUND_QUEUE_3v3_SOLO);
		//BattlegroundQueue* queue = &sBattlegroundMgr->m_BattlegroundQueues[BATTLEGROUND_QUEUE_3v3_SOLO];

		for (int i = 0; i < MAX_TALENT_CAT; i++)
			cache3v3Queue[i] = 0;

		for (int i = BG_BRACKET_ID_FIRST; i <= BG_BRACKET_ID_LAST; i++)
			for (int j = 0; j < 2; j++)
			{

				for (BattlegroundQueue::GroupsQueueType::iterator itr = queue->m_QueuedGroups[i][j].begin(); itr != queue->m_QueuedGroups[i][j].end(); itr++)
				{
					if ((*itr)->IsInvitedToBGInstanceGUID) // Skip when invited
						continue;

                    GroupQueueInfo* groupQueue = (*itr);
                    std::set<uint64> *players = &(*itr)->Players;
                    for (std::set<uint64>::iterator it = players->begin(); it != players->end(); it++)
                    {
                        uint64 guid = *it;
                        Player* plr = sObjectAccessor->FindPlayer(guid);
                        if (!plr)
                            continue;

						Solo3v3TalentCat plrCat = GetTalentCatForSolo3v3(plr); // get talent cat
						cache3v3Queue[plrCat]++;
					}
				}
			}
	}
};

class SoloQ3v3World : public WorldScript
{
public:
    SoloQ3v3World() : WorldScript("SoloQ3v3World") { }

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload) {
            std::string cfg_file = "3v3_soloq.conf";
            std::string cfg_def_file = cfg_file +".dist";

            sConfigMgr->LoadMore(cfg_def_file.c_str());

            sConfigMgr->LoadMore(cfg_file.c_str());
        }
    }
};


void AddSC_npc_solo3v3()
{
    new SoloQ3v3World();
	new npc_solo3v3();
}