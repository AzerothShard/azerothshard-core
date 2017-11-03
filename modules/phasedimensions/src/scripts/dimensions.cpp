#include "ScriptMgr.h"
#include "Player.h"
#include "Item.h"
#include "ConditionMgr.h"
#include "ScriptedGossip.h"
#include "GossipDef.h"
#include "AzthLanguage.h"
#include "AzthUtils.h"
#include "AzthSharedDefines.h"

class PhaseDimensionsNPC : public CreatureScript
{
public:
    PhaseDimensionsNPC() : CreatureScript("PhaseDimensionsNPC") {}

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        uint32 currDimension=player->azthPlayer->getCurrentDimensionByMark();
        std::string msg="Attualmente sei nella dimensione: ";
        std::string dimName= sAzthUtils->getDimensionName(currDimension);
        msg +=  dimName;

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, msg.c_str(), GOSSIP_SENDER_MAIN, 0);

        if (currDimension != uint32(DIMENSION_NORMAL))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "[PRINCIPALE] Illusory World: Dimensione standard.", GOSSIP_SENDER_MAIN, DIMENSION_NORMAL);

        if (currDimension != uint32(DIMENSION_GUILD))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Guild World: Dimensione delle case di gilda.", GOSSIP_SENDER_MAIN, DIMENSION_GUILD);

        if (currDimension != uint32(DIMENSION_PVP))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Corrupted World: Dimensione riservata al pvp", GOSSIP_SENDER_MAIN, DIMENSION_PVP);

        if (currDimension != uint32(DIMENSION_ENTERTAINMENT))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Toys Land: Dimensione dedicata agli eventi di intrattenimento", GOSSIP_SENDER_MAIN, DIMENSION_ENTERTAINMENT);

        if (currDimension != uint32(DIMENSION_RPG))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Warcraft Tales: Dimensione dedicata al GDR", GOSSIP_SENDER_MAIN, DIMENSION_RPG);

        if (currDimension != uint32(DIMENSION_60))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Classic World: Dimensione con livello limite 60", GOSSIP_SENDER_MAIN, DIMENSION_60);

        if (currDimension != uint32(DIMENSION_70))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "TBC World: Dimensione con livello limite 70", GOSSIP_SENDER_MAIN, DIMENSION_70);
        
        if (currDimension != uint32(DIMENSION_TEST))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Test World: Dimensione riservata ai test (siate cauti!)", GOSSIP_SENDER_MAIN, DIMENSION_TEST);
        
        if (currDimension != uint32(DIMENSION_GM) && player->GetSession()->GetSecurity() > SEC_PLAYER)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "GameMaster World: Benvenuti GM e Developers, questo è il vostro mondo!", GOSSIP_SENDER_MAIN, DIMENSION_GM);

        player->SEND_GOSSIP_MENU(1, creature->GetGUID());
        return true;
    }


    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->SendCloseGossip();
        
        if (!action)
            return true;
        
        if (sender == GOSSIP_SENDER_MAIN)
        {           
            player->azthPlayer->changeDimension(action, true);
        }

        return true;
    }
};

class PhaseDimensions : public PlayerScript
{
public:
    PhaseDimensions() : PlayerScript("PhaseDimensions") {}

    void OnMapChanged(Player* player) override { 
        if (!player)
            return;
        
        uint32 curDimension   = player->azthPlayer->getCurrentDimensionByMark();
        uint32 aurDimension   = player->azthPlayer->getCurrentDimensionByAura();
        // we should make an integrity check with phase
        // avoiding cases where we're on a special dimension
        // without aura (hacking/exploit)
        uint32 phaseDimension = player->azthPlayer->getCurrentDimensionByPhase();
        
        MapEntry const* mEntry = sMapStore.LookupEntry(player->GetMapId());
        
        if (sAzthUtils->isPhasedDimension(curDimension) && (mEntry->IsBattlegroundOrArena() || mEntry->IsDungeon() || mEntry->IsRaid())) {
            // TEMPORARY DISABLE DIMENSION IN BG BUT DO NOT REMOVE THE MARKER
            player->azthPlayer->changeDimension(DIMENSION_NORMAL, false, true);
        } else if (phaseDimension == DIMENSION_60 && mEntry->Expansion() > 0) {
            // CLASSIC EXPANSION CHECK
            player->TeleportTo(AzthSharedDef::blackMarket);
            ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000 Sei nella dimensione Classic, non è possibile accedere a mappe di espansioni più recenti|r");
        } else if (phaseDimension == DIMENSION_70 && mEntry->Expansion() > 1) {
            // TBC EXPANSION CHECK
            player->TeleportTo(AzthSharedDef::blackMarket);
            ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000 Sei nella dimensione TBC, non è possibile accedere a mappe di espansioni più recenti|r");
        } else if (phaseDimension == DIMENSION_GM && player->GetSession()->GetSecurity() > SEC_PLAYER) {
            // GM security check
            player->TeleportTo(AzthSharedDef::blackMarket);
            player->azthPlayer->changeDimension(DIMENSION_NORMAL);
            ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000 Non puoi accedere alla dimensione dei Game Masters!|r");
        } else if (phaseDimension == DIMENSION_GUILD && !player->GetGuild()) {
            // GUILD security check
            player->TeleportTo(AzthSharedDef::blackMarket);
            player->azthPlayer->changeDimension(DIMENSION_NORMAL);
            ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000 Non puoi accedere a questa dimensione senza far parte di una gilda!|r");
        } else {
            // integrity check: re-enable temporary disabled dimensions or just fix possible exploits
            if (curDimension != aurDimension || curDimension != phaseDimension) {
                player->azthPlayer->changeDimension(curDimension);
            }
        }
    }
    
};

void AddSC_PhaseDimensions()
{
    new PhaseDimensions();
    new PhaseDimensionsNPC();
}