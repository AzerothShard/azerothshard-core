#include "ScriptMgr.h"
#include "Player.h"
#include "Item.h"
#include "ItemPrototype.h"
#include "Common.h"
#include "music.h"
#include "ScriptedCreature.h"

std::vector<music> musicList;

class loadMusic : public WorldScript
{
public:
    loadMusic() : WorldScript("loadMusic") {}
    void OnStartup()
    {
        QueryResult music_table = ExtraDatabase.PQuery("SELECT * FROM jukebox_music ORDER BY author ASC, title;"); //author, title and music id from DB
        if (!music_table)
        {
            sLog->outString(">> Loaded 0 musics for jukebox. DB table `jukebox_music` is empty.");
            sLog->outString();
            return;
        }

        Field* musicField = music_table->Fetch();

        do {
            musicList.push_back(music(musicField[0].GetString(), musicField[1].GetString(), musicField[2].GetUInt32())); //push data into musicList array
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
                events.ScheduleEvent(EVENT_JUKEBOX_START, 10 * 60 * IN_MILLISECONDS, 1, 0);
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
        if (creature->AI()->GetData(DATA_JUKEBOX_READY) > 0) {
            for (uint32 i = 0; i < musicList.size(); i++)
            {
                player->ADD_GOSSIP_ITEM(0, musicList[i].GetAuthor() + " - " + musicList[i].GetTitle(), GOSSIP_SENDER_MAIN, i);
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
            if (action <= musicList.size())
            {
                creature->PlayDistanceSound(musicList[action].GetMusicId());
                std::string msg = "Now playing: " + musicList[action].GetAuthor() + " - " + musicList[action].GetTitle();
                creature->MonsterSay(msg.c_str(), 0, creature);
                creature->AI()->DoAction(DATA_JUKEBOX_READY);
            }
            player->PlayerTalkClass->SendCloseGossip();
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

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets)
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