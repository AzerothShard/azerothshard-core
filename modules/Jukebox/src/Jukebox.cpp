#include "ScriptMgr.h"
#include "Player.h"
#include "Item.h"
#include "ItemPrototype.h"
#include "Common.h"
#include "music.h"

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


class Jukebox : public CreatureScript
{
public:
  Jukebox() : CreatureScript("Jukebox") {}
  
  bool OnGossipHello(Player* player, Creature* creature)
  {
	  for (uint32 i = 0; i < musicList.size(); i++)
	  {
		  player->ADD_GOSSIP_ITEM(0, musicList[i].GetAuthor() + " - " + musicList[i].GetTitle(), GOSSIP_SENDER_MAIN, i);
	  }
	  player->SEND_GOSSIP_MENU(1, creature->GetGUID());
	  return true;
  }


  bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
  {
	  player->PlayerTalkClass->ClearMenus();
	  if (action <= musicList.size())
	  {
		  creature->PlayDistanceSound(musicList[action].GetMusicId());
	  }
	  player->PlayerTalkClass->SendCloseGossip();
	  return true;
  }
};


void AddSC_Jukebox()
{
	new loadMusic();
	new Jukebox();
}