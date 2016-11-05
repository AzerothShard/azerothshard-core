/*
 * Originally written by Xinef - Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: http://github.com/azerothcore/azerothcore-wotlk/LICENSE-AGPL

REWRITTEN BY XINEF
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SmartAI.h"

enum AshbringerEventMisc
{
    AURA_OF_ASHBRINGER = 28282,
    NPC_SCARLET_MYRIDON = 4295,
    NPC_SCARLET_DEFENDER = 4298,
    NPC_SCARLET_CENTURION = 4301,
    NPC_SCARLET_SORCERER = 4294,
    NPC_SCARLET_WIZARD = 4300,
    NPC_SCARLET_ABBOT = 4303,
    NPC_SCARLET_MONK = 4540,
    NPC_SCARLET_CHAMPION = 4302,
    NPC_SCARLET_CHAPLAIN = 4299,
    NPC_FAIRBANKS = 4542,
    FACTION_FRIENDLY_TO_ALL = 35,
};

class instance_scarlet_monastery : public InstanceMapScript
{
public:
    instance_scarlet_monastery() : InstanceMapScript("instance_scarlet_monastery", 189) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_scarlet_monastery_InstanceMapScript(map);
    }

    struct instance_scarlet_monastery_InstanceMapScript : public InstanceScript
    {
        instance_scarlet_monastery_InstanceMapScript(Map* map) : InstanceScript(map) {}

        void OnPlayerEnter(Player* player)
        {
            if (player->HasAura(AURA_OF_ASHBRINGER))
            {
                std::list<Creature*> ScarletList;
                player->GetCreatureListWithEntryInGrid(ScarletList, NPC_SCARLET_MYRIDON, 2000.0f);
                player->GetCreatureListWithEntryInGrid(ScarletList, NPC_SCARLET_DEFENDER, 2000.0f);
                player->GetCreatureListWithEntryInGrid(ScarletList, NPC_SCARLET_CENTURION, 2000.0f);
                player->GetCreatureListWithEntryInGrid(ScarletList, NPC_SCARLET_SORCERER, 2000.0f);
                player->GetCreatureListWithEntryInGrid(ScarletList, NPC_SCARLET_WIZARD, 2000.0f);
                player->GetCreatureListWithEntryInGrid(ScarletList, NPC_SCARLET_ABBOT, 2000.0f);
                player->GetCreatureListWithEntryInGrid(ScarletList, NPC_SCARLET_MONK, 2000.0f);
                player->GetCreatureListWithEntryInGrid(ScarletList, NPC_SCARLET_CHAMPION, 2000.0f);
                player->GetCreatureListWithEntryInGrid(ScarletList, NPC_SCARLET_CHAPLAIN, 2000.0f);
                player->GetCreatureListWithEntryInGrid(ScarletList, NPC_FAIRBANKS, 2000.0f);
                if (!ScarletList.empty())
                    for (std::list<Creature*>::iterator itr = ScarletList.begin(); itr != ScarletList.end(); itr++)
                        (*itr)->setFaction(FACTION_FRIENDLY_TO_ALL);
            }
        }
    };
};

enum ScarletMonasteryTrashMisc
{
    SAY_WELCOME = 0,
    AURA_ASHBRINGER = 28282,
    //FACTION_FRIENDLY_TO_ALL = 35,
    NPC_HIGHLORD_MOGRAINE = 16440,
    SPELL_COSMETIC_CHAIN = 45537,
    SPELL_COSMETIC_EXPLODE = 45935,
};

class npc_scarlet_guard : public CreatureScript
{
public:
    npc_scarlet_guard() : CreatureScript("npc_scarlet_guard") { }

    struct npc_scarlet_guardAI : public SmartAI
    {
        npc_scarlet_guardAI(Creature* creature) : SmartAI(creature) { }

        void Reset()
        {
            SayAshbringer = false;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (who && who->GetDistance2d(me) < 10.0f)
                if (Player* player = who->ToPlayer())
                    if (player->HasAura(AURA_ASHBRINGER) && !SayAshbringer)
                    {
                        Talk(SAY_WELCOME);
                        me->setFaction(FACTION_FRIENDLY_TO_ALL);
                        me->SetSheath(SHEATH_STATE_UNARMED);
                        me->SetFacingToObject(player);
                        me->SetStandState(UNIT_STAND_STATE_KNEEL);
                        SayAshbringer = true;
                    }

            SmartAI::MoveInLineOfSight(who);
        }
    private:
        bool SayAshbringer;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scarlet_guardAI(creature);
    }
};

class npc_mograine : public CreatureScript
{
public:
    npc_mograine() : CreatureScript("npc_scarlet_commander_mograine") { }

    bool OnGossipHello(Player* plr, Creature* npc)
    {
        plr->ADD_GOSSIP_ITEM(0, "The Ashbringer?", GOSSIP_SENDER_MAIN, 1);
        npc->SetFacingToObject(plr);
        plr->SEND_GOSSIP_MENU(500000, npc->GetGUID());
        return true;
    }

    void Mograine(Creature* summoner, uint32 summonedid)
    {
        summoner->SummonCreature(summonedid, 1148.068237f, 1398.511963f, 31.972404f, 6.280230f, TEMPSUMMON_TIMED_DESPAWN, 400000);
    }

    bool OnGossipSelect(Player* plr, Creature* npc, uint32 Sender, uint32 uiAction)
    {
        Creature* mograine;
        plr->PlayerTalkClass->ClearMenus();

        switch (uiAction)
        {
        case 1:
            npc->MonsterSay("You hold my father's blade, $N. My soldiers are yours to control, my $GLord:Lady;. Take them... Lead them... The impure must be purged. They must be cleansed of their taint.", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "Oh?", GOSSIP_SENDER_MAIN, 2);
            plr->SEND_GOSSIP_MENU(500001, npc->GetGUID());
            return true;
        case 2:
            Mograine(npc, NPC_HIGHLORD_MOGRAINE);
            mograine = npc->FindNearestCreature(NPC_HIGHLORD_MOGRAINE, 200.0f);
            mograine->setFaction(FACTION_FRIENDLY_TO_ALL);
            mograine->MonsterSay("Renault...", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "Who's that?", GOSSIP_SENDER_MAIN, 3);
            plr->SEND_GOSSIP_MENU(500002, npc->GetGUID());
            return true;
        case 3:
            mograine = npc->FindNearestCreature(NPC_HIGHLORD_MOGRAINE, 200.0f);
            npc->SetFacingToObject(mograine);
            npc->SetStandState(UNIT_STAND_STATE_STAND);
            npc->MonsterSay("Father... But... How?", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "Father!?", GOSSIP_SENDER_MAIN, 4);
            plr->SEND_GOSSIP_MENU(500003, npc->GetGUID());
            return true;
        case 4:
            mograine = npc->FindNearestCreature(NPC_HIGHLORD_MOGRAINE, 200.0f);
            npc->SetFacingToObject(mograine);
            mograine->MonsterSay("Did you think that your betrayal would be forgotten? Lost in the carefully planned cover up of my death? Blood of my blood, the blade felt your cruelty long after my heart had stopped beating. And in death, I knew what you had done. But now, the chains of Kel'Thuzad hold me no more. I come to serve justice. I AM ASHBRINGER.", LANG_UNIVERSAL, plr);
            mograine->HandleEmoteCommand(EMOTE_ONESHOT_BATTLE_ROAR);
            npc->SetFacingToObject(mograine);
            plr->ADD_GOSSIP_ITEM(0, "What betrayal, Commander?", GOSSIP_SENDER_MAIN, 5);
            plr->SEND_GOSSIP_MENU(500004, npc->GetGUID());
            return true;
        case 5:
            mograine = npc->FindNearestCreature(NPC_HIGHLORD_MOGRAINE, 200.0f);
            npc->SetFacingToObject(mograine);
            npc->SetSheath(SHEATH_STATE_UNARMED);
            npc->HandleEmoteCommand(EMOTE_ONESHOT_BEG);
            npc->MonsterSay("Forgive me, father! Please...", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "Mograine!", GOSSIP_SENDER_MAIN, 6);
            plr->SEND_GOSSIP_MENU(500005, npc->GetGUID());
            return true;
        case 6:
            mograine = npc->FindNearestCreature(NPC_HIGHLORD_MOGRAINE, 200.0f);
            npc->SetFacingToObject(mograine);
            mograine->CastSpell(npc, SPELL_COSMETIC_CHAIN);
            npc->CastSpell(npc, SPELL_COSMETIC_EXPLODE);
            mograine->MonsterSay(" You are forgiven...", LANG_UNIVERSAL, plr);
            npc->Kill(mograine, npc);
            mograine->DespawnOrUnsummon(0.5 * IN_MILLISECONDS);
            plr->CLOSE_GOSSIP_MENU();
            return true;
        }
    }

    struct npc_mograineAI : public SmartAI
    {
        npc_mograineAI(Creature* creature) : SmartAI(creature) { }

        void Reset()
        {
            SayAshbringer = false;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (who && who->GetDistance2d(me) < 10.0f)
                if (Player* player = who->ToPlayer())
                    if (player->HasAura(AURA_ASHBRINGER) && !SayAshbringer)
                    {
                        me->setFaction(FACTION_FRIENDLY_TO_ALL);
                        me->SetSheath(SHEATH_STATE_UNARMED);
                        me->SetStandState(UNIT_STAND_STATE_KNEEL);
                        me->SetFacingToObject(player);
                        me->MonsterYell("Bow down! Kneel before the Ashbringer! A new dawn approaches, brothers and sisters! Our message will be delivered to the filth of this world through the chosen one!", LANG_UNIVERSAL, player);
                        SayAshbringer = true;
                    }

            SmartAI::MoveInLineOfSight(who);
        }
    private:
        bool SayAshbringer;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mograineAI(creature);
    }
};
class npc_fairbanks : public CreatureScript
{
public:
    npc_fairbanks() : CreatureScript("npc_fairbanks") { }

    bool OnGossipHello(Player* plr, Creature* npc)
    {
        plr->ADD_GOSSIP_ITEM(0, "Curse? What's going on here, Fairbanks?", GOSSIP_SENDER_MAIN, 1);
        plr->SEND_GOSSIP_MENU(600016, npc->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* plr, Creature* npc, uint32 Sender, uint32 uiAction)
    {
        plr->PlayerTalkClass->ClearMenus();

        switch (uiAction)
        {
        case 1:
            npc->MonsterSay("You mean, you don't know? The sword that you carry on your back - it is known as Ashbringer; named after its original owner.", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "Mograine?", GOSSIP_SENDER_MAIN, 2);
            plr->SEND_GOSSIP_MENU(600001, npc->GetGUID());
            return true;
        case 2:
            npc->MonsterSay("Aye, the Highlord Mograine: A founder of the original order of the Scarlet Crusade. A knight of unwavering faith and purity; Mograine would be betrayed by his own son and slain by Kel'Thuzad's forces inside Stratholme. It is how I ended up here...", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "What do you mean?", GOSSIP_SENDER_MAIN, 3);
            plr->SEND_GOSSIP_MENU(600002, npc->GetGUID());
            return true;
        case 3:
            npc->MonsterSay("It was High General Abbendis, High Inquisitor Isillien, and Highlord Mograine that formed the Crusade. In its infancy, the Crusade was a noble order. The madness and insane zealotry that you see now did not exist. It was not until the one known as Grand Crusader appeared that the wheels of corruption were set in motion.", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "I still do not fully understand.", GOSSIP_SENDER_MAIN, 4);
            plr->SEND_GOSSIP_MENU(600003, npc->GetGUID());
            return true;
        case 4:
            npc->MonsterSay("The Highlord was the lynchpin of the Crusade. Aye, Mograine was called the Ashbringer because of his exploits versus the armies of the Lich King. With only blade and faith, Mograine would walk into whole battalions of undead and emerge unscathed - the ashes of his foes being the only indication that he had been there at all. Do you not understand? The very face of death feared him! It trembled in his presence!", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "Incredible story. So how did he die?", GOSSIP_SENDER_MAIN, 5);
            plr->SEND_GOSSIP_MENU(600004, npc->GetGUID());
            return true;
        case 5:
            npc->MonsterSay("The only way a hero can die, $R: Through tragedy. The Grand Crusader struck a deal with Kel'Thuzad himself! An ambush would be staged that would result in the death of Mograine. The type of betrayal that could only be a result of the actions of one's most trusted and loved companions.", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "You mean...", GOSSIP_SENDER_MAIN, 6);
            plr->SEND_GOSSIP_MENU(600005, npc->GetGUID());
            return true;
        case 6:
            npc->MonsterTextEmote("High Inquisitor Fairbanks nods.", plr);
            npc->MonsterSay("Aye, the lesser Mograine, the one known as the Scarlet Commander, through - what I suspect - the dealings of the Grand Crusader. He led his father to the ambush like a lamb to the slaughter.", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "How do you know all of this?", GOSSIP_SENDER_MAIN, 7);
            plr->SEND_GOSSIP_MENU(600006, npc->GetGUID());
            return true;
        case 7:
            npc->MonsterTextEmote("High Inquisitor Fairbanks lifts up his tabard revealing several gruesome scars.", plr);
            npc->MonsterSay("Because I was there... I was the Highlord's most trusted advisor. I should have known... I felt that something was amiss yet I allowed it to happen. Would you believe that there were a thousand or more Scourge?", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "A thousand? For one man?", GOSSIP_SENDER_MAIN, 8);
            plr->SEND_GOSSIP_MENU(600007, npc->GetGUID());
            return true;
        case 8:
            npc->MonsterSay("This was the Ashbringer, fool! As the Scourge began to materialize around us, Mograine's blade began to glow... to hum... the younger Mograine would take that as a sign to make his escape. They descended upon us with a hunger the likes of which I had never seen. Yet...", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "Yet? Yet what?", GOSSIP_SENDER_MAIN, 9);
            plr->SEND_GOSSIP_MENU(600008, npc->GetGUID());
            return true;
        case 9:
            npc->MonsterSay("It was not enough.", LANG_UNIVERSAL, plr);
            npc->MonsterTextEmote("Fairbanks smirks briefly, lost in a memory.", plr);
            npc->MonsterSay("A thousand came and a thousand died. By the Light! By the might of Mograine! He would smite them down as fast as they could come. Through the chaos, I noticed that the lesser Mograine was still there, off in the distance. I called him, 'Help us, Renault! Help your father, boy!'", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "And did he?", GOSSIP_SENDER_MAIN, 10);
            plr->SEND_GOSSIP_MENU(600009, npc->GetGUID());
            return true;
        case 10:
            npc->MonsterTextEmote("High Inquisitor Fairbanks shakes his head.", plr);
            npc->MonsterSay("No... He stood in the background, watching as the legion of undead descended upon us. Soon after, my powers were exhausted. I was the first to fall... Surely they would tear me limb from limb as I lay there unconscious; but they ignored me completely, focusing all of their attention on the Highlord.", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "Continue please, Fairbanks.", GOSSIP_SENDER_MAIN, 11);
            plr->SEND_GOSSIP_MENU(600010, npc->GetGUID());
            return true;
        case 11:
            npc->MonsterSay("It was all I could do to feign death as the corpses of the Scourge piled upon me. There was darkness and only the muffled sounds of the battle above me. The clashing of iron, the gnashing and grinding... gruesome, terrible sounds. And then there was silence. He called to me! 'Fairbanks!Fairbanks, where are you ? Talk to me, Fairbanks!' And then came the sound of incredulousness. The bite of betrayal, $R...", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "You mean...", GOSSIP_SENDER_MAIN, 12);
            plr->SEND_GOSSIP_MENU(600011, npc->GetGUID());
            return true;
        case 12:
            npc->MonsterSay("The boy had picked up Ashbringer and driven it through his father's heart as his back was turned. His last words will haunt me forever: 'What have you done, Renault? Why would you do this?'", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "You were right, Fairbanks. That is tragic.", GOSSIP_SENDER_MAIN, 13);
            plr->SEND_GOSSIP_MENU(600012, npc->GetGUID());
            return true;
        case 13:
            npc->MonsterSay("The blade and Mograine were a singular entity. Do you understand? This act corrupted the blade and lead to Mograine's own corruption as a death knight of Kel'Thuzad. I swore that if I lived, I would expose the perpetrators of this heinous crime. For two days I remained under the rot and contagion of Scourge - gathering as much strength as possible to escape the razed city.", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "And you did...", GOSSIP_SENDER_MAIN, 14);
            plr->SEND_GOSSIP_MENU(600013, npc->GetGUID());
            return true;
        case 14:
            npc->MonsterSay("Aye, I did. Much to the dismay of the lesser Mograine, I made my way back to the Scarlet Monastery. I shouted and screamed. I told the tale to any that would listen. And I would be murdered in cold blood for my actions, dragged to this chamber - the dark secret of the order. But some did listen... some heard my words. Thus was born the Argent Dawn...", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "You tell an incredible tale, Fairbanks. What of the blade? Is it beyond redemption?", GOSSIP_SENDER_MAIN, 15);
            plr->SEND_GOSSIP_MENU(600014, npc->GetGUID());
            return true;
        case 15:
            npc->MonsterSay("I'm afraid that the blade which you hold in your hands is beyond saving. The hatred runs too deep. But do not lose hope, hero. Where one chapter has ended, a new one begins. $B Find his son - a more devout and pious man you may never meet.It is rumored that he is able to build the Ashbringer anew, without requiring the old, tainted blade.", LANG_UNIVERSAL, plr);
            plr->ADD_GOSSIP_ITEM(0, "But his son is dead.", GOSSIP_SENDER_MAIN, 16);
            plr->SEND_GOSSIP_MENU(600015, npc->GetGUID());
            return true;
        case 16:
            npc->MonsterTextEmote("High Inquisitor Fairbanks shakes his head.", plr);
            npc->MonsterSay("No, $C; only one of his sons is dead. The other lives...", LANG_UNIVERSAL, plr);
            npc->MonsterTextEmote("High Inquisitor Fairbanks points to the sky", plr);
            npc->MonsterSay("The Plaguelands... Find him there...", LANG_UNIVERSAL, plr);
            plr->CLOSE_GOSSIP_MENU();
            return true;
        }

        return true;
    }

    struct npc_fairbanksAI : public SmartAI
    {
        npc_fairbanksAI(Creature* creature) : SmartAI(creature) { }

        void Reset()
        {
            SayAshbringer = false;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (who && who->GetDistance2d(me) < 10.0f)
                if (Player* player = who->ToPlayer())
                    if (player->HasAura(AURA_ASHBRINGER) && !SayAshbringer)
                    {
                        me->setFaction(FACTION_FRIENDLY_TO_ALL);
                        me->SetSheath(SHEATH_STATE_UNARMED);
                        me->MonsterSay("At last, the curse is lifted. Thank you, hero.", LANG_UNIVERSAL, player);
                        me->SetDisplayId(16179);
                        me->SetFacingToObject(player);
                        SayAshbringer = true;
                    }

            SmartAI::MoveInLineOfSight(who);
        }
    private:
        bool SayAshbringer;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fairbanksAI(creature);
    }
};

void AddSC_instance_scarlet_monastery()
{
    new instance_scarlet_monastery();
    new npc_scarlet_guard();
    new npc_fairbanks();
    new npc_mograine();
}
