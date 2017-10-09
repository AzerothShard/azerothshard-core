#include "ScriptMgr.h"
#include "Player.h"
#include "Item.h"
#include "ItemPrototype.h"
#include "Common.h"
#include "music.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"

std::map<string, std::map<int, music>> musicList;
std::vector<string> genresName;

class loadMusic : public WorldScript
{
public:
    loadMusic() : WorldScript("loadMusic") {}
    void OnStartup()
    {
        QueryResult music_table = ExtraDatabase.PQuery("SELECT * FROM jukebox_music ORDER BY author ASC, title;"); //author, title, music id and genre from DB
        if (!music_table)
        {
            sLog->outString(">> Loaded 0 musics for jukebox. DB table `jukebox_music` is empty.\n");
            sLog->outString();
            return;
        }
        Field* musicField = music_table->Fetch();

        uint32 t = 1;
        do
        {
            musicList[musicField[3].GetString()][t] = music(musicField[0].GetString(), musicField[1].GetString(), musicField[2].GetUInt32(), musicField[3].GetString());
            t++;
        } while (music_table->NextRow());
    }
};

enum npc_jukebox_enum
{
    EVENT_JUKEBOX_START = 1,

    DATA_JUKEBOX_READY = 1,
};


class Jukebox : public CreatureScript
{
public:
    Jukebox() : CreatureScript("Jukebox") {}

    struct npc_jukeboxAI : public ScriptedAI
    {
        npc_jukeboxAI(Creature *c) : ScriptedAI(c)
        {
            events.Reset();
            jukeboxReady = true;
        }

        EventMap events;
        bool jukeboxReady;
		bool enableUpdateAI = true;

        uint32 GetData(uint32 type) const
        {
            if (type == DATA_JUKEBOX_READY)
                return (uint32)jukeboxReady;

            return 0;
        }

        void DoAction(int32 param)
        {
            if (param == DATA_JUKEBOX_READY) {
                jukeboxReady = false;
                events.ScheduleEvent(EVENT_JUKEBOX_START, 3 * 60 * IN_MILLISECONDS, 1, 0);
            }
        }

        void UpdateAI(uint32 diff)
        {
            events.Update(diff);
			switch (events.GetEvent())
			{
				case EVENT_JUKEBOX_START:
				{
					jukeboxReady = true;
					events.Reset();
				}
			}
        }
    };

    bool OnGossipHello(Player* player, Creature* creature)
    {
        uint32 genresCounter = 0;
        if (creature->AI()->GetData(DATA_JUKEBOX_READY) > 0) {
            for (std::map<string, std::map<int, music>>::iterator it = musicList.begin(); it != musicList.end(); it++)
            {
                string genre = it->first;
                player->ADD_GOSSIP_ITEM(0, genre, GOSSIP_SENDER_MAIN, genresCounter);
                genresName.push_back(genre);
                genresCounter++;
            }
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());
            return true;
        } else {
			player->SEND_GOSSIP_MENU(50002, creature->GetGUID());
            return true;
        }
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        if (creature->AI()->GetData(DATA_JUKEBOX_READY) > 0) {
            player->PlayerTalkClass->ClearMenus();

            if (action < 1000)
            {
                for (std::map<int, music>::iterator it = musicList[genresName[action]].begin(); it != musicList[genresName[action]].end(); it++)
                {
                    player->ADD_GOSSIP_ITEM(0, it->second.GetAuthor() + " - " + it->second.GetTitle(), GOSSIP_SENDER_MAIN, ((action+1)*1000)+ it->first); //action +1 to avoid 0*1000 = 0
                    player->SEND_GOSSIP_MENU(1, creature->GetGUID());
                }
            }
            else
            {
                string genre = genresName[((int)((action) / 1000))-1]; // -1 to revert action +1 did before
                uint32 id = action - (int)((action) / 1000) * 1000;
                creature->PlayDistanceSound(musicList[genre][id].GetMusicId());
                std::string msg = "Now playing: " + musicList[genre][id].GetAuthor() + " - " + musicList[genre][id].GetTitle();
                creature->MonsterSay(msg.c_str(), 0, creature);
                creature->AI()->DoAction(DATA_JUKEBOX_READY);
                player->PlayerTalkClass->SendCloseGossip();
            }
            return true;
        }
        else {
            return false;
        }
    }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_jukeboxAI(pCreature);
    }
};

class JukeboxSummon : public ItemScript
{
public:
    JukeboxSummon() : ItemScript("JukeboxSummon") {}

    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/)
    {
        player->SummonCreature(300205, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 10 * 60 * IN_MILLISECONDS);
        return true;
    }
};


void AddSC_Jukebox()
{
    new loadMusic();
    new Jukebox();
    new JukeboxSummon();
}