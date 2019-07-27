#include "ScriptMgr.h"
#include "Player.h"
#include "Item.h"
#include "ConditionMgr.h"
#include "ScriptedGossip.h"
#include "GossipDef.h"
#include "AzthLanguageStrings.h"
#include "AzthUtils.h"
#include "AzthSharedDefines.h"
#include "AZTH.h"

class PhaseDimensionsNPC : public CreatureScript
{
public:
    PhaseDimensionsNPC() : CreatureScript("PhaseDimensionsNPC") {}

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        uint32 currDimension=sAZTH->GetAZTHPlayer(player)->getCurrentDimensionByMark();
        std::string msg=sAzthLang->get(AZTH_LANG_DIMENSION_CURRENT, player);
        std::string dimName= sAzthUtils->getDimensionName(currDimension);
        msg +=  dimName;

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, msg.c_str(), GOSSIP_SENDER_MAIN, 0);


        if (!sAZTH->GetAZTHPlayer(player)->isPvP() && currDimension != uint32(DIMENSION_NORMAL))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, sAzthLang->get(AZTH_LANG_DIMENSION_ILLUSORY, player), GOSSIP_SENDER_MAIN, DIMENSION_NORMAL);

        if (currDimension != uint32(DIMENSION_GUILD))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, sAzthLang->get(AZTH_LANG_DIMENSION_GUILD, player), GOSSIP_SENDER_MAIN, DIMENSION_GUILD);

        if (currDimension != uint32(DIMENSION_PVP))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, sAzthLang->get(AZTH_LANG_DIMENSION_PVP, player), GOSSIP_SENDER_MAIN, DIMENSION_PVP);

        if (currDimension != uint32(DIMENSION_ENTERTAINMENT))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, sAzthLang->get(AZTH_LANG_DIMENSION_ENTERTAIN, player), GOSSIP_SENDER_MAIN, DIMENSION_ENTERTAINMENT);

        if (currDimension != uint32(DIMENSION_RPG))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, sAzthLang->get(AZTH_LANG_DIMENSION_GDR, player), GOSSIP_SENDER_MAIN, DIMENSION_RPG);

        if (currDimension != uint32(DIMENSION_60))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, sAzthLang->get(AZTH_LANG_DIMENSION_LVL60, player), GOSSIP_SENDER_MAIN, DIMENSION_60);

        if (currDimension != uint32(DIMENSION_70))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, sAzthLang->get(AZTH_LANG_DIMENSION_LVL70, player), GOSSIP_SENDER_MAIN, DIMENSION_70);
        
        if (currDimension != uint32(DIMENSION_TEST))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, sAzthLang->get(AZTH_LANG_DIMENSION_TEST, player), GOSSIP_SENDER_MAIN, DIMENSION_TEST);
        
        if (currDimension != uint32(DIMENSION_GM) && player->GetSession()->GetSecurity() > SEC_PLAYER)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, sAzthLang->get(AZTH_LANG_DIMENSION_GM, player), GOSSIP_SENDER_MAIN, DIMENSION_GM);

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
            sAZTH->GetAZTHPlayer(player)->changeDimension(action, true);
        }

        return true;
    }
};

class PhaseDimensions : public PlayerScript
{
public:
    PhaseDimensions() : PlayerScript("PhaseDimensions") {}

    bool OnBeforeTeleport(Player* player, uint32 /*mapid*/, float /*x*/, float /*y,*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit *target) override
    {
        if (!target)
            return true;

        if (target->GetTypeId() != TYPEID_PLAYER)
            return true;

        Player *pTarget = target->ToPlayer();

        if (player->GetGUIDLow() == pTarget->GetGUIDLow())
            return true; //strange case but could happen

        uint32 playerPhaseDimension = sAZTH->GetAZTHPlayer(player)->getCurrentDimensionByPhase();
        uint32 targetPhaseDimension = sAZTH->GetAZTHPlayer(pTarget)->getCurrentDimensionByPhase();
        
        if (targetPhaseDimension == playerPhaseDimension)
            return true;

        if (targetPhaseDimension == DIMENSION_NORMAL || player->IsGameMaster()) {               
            if (!sAZTH->GetAZTHPlayer(player)->changeDimension(targetPhaseDimension, true))
                return false;
        } else {
            std::string dimSource = sAzthUtils->getDimensionName(playerPhaseDimension);
            std::string dimDest = sAzthUtils->getDimensionName(targetPhaseDimension);

            ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_MULTIDIMENSION_CANNOT_BE_SUMMONED, player));
            ChatHandler(pTarget->GetSession()).SendSysMessage(sAzthLang->getf(AZTH_LANG_MULTIDIMENSION_CANNOT_BE_SUMMONED_SUMMONER, player, dimSource.c_str(), dimDest.c_str()));
            return false;
        }

        return true;
    }

    void OnMapChanged(Player* player) override { 
        _zoneCheck(player, player->GetMapId(), player->GetZoneId(), player->GetAreaId());
    }
    
    void OnUpdateZone(Player *player, uint32 zone, uint32 area) override {
        AreaTableEntry const* aEntry = sAreaTableStore.LookupEntry(area);
        if (!aEntry)
            return;

        _zoneCheck(player, aEntry->mapid , zone, area);
    }
    
    
    void _zoneCheck(Player *player, uint32 mapid, uint32 zoneid, uint32 areaid) {
        if (!player)
            return;
        
        uint32 curDimension   = sAZTH->GetAZTHPlayer(player)->getCurrentDimensionByMark();
        uint32 aurDimension   = sAZTH->GetAZTHPlayer(player)->getCurrentDimensionByAura();
        // we should make an integrity check with phase
        // avoiding cases where we're on a special dimension
        // without aura (hacking/exploit)
        uint32 phaseDimension = sAZTH->GetAZTHPlayer(player)->getCurrentDimensionByPhase();
        
        MapEntry const* mEntry = sMapStore.LookupEntry(mapid);
        
        if (sAzthUtils->isPhasedDimension(curDimension) && sAzthUtils->isSharedArea(player, mEntry, zoneid, areaid)) {
            // TEMPORARY DISABLE DIMENSION IN BG BUT DO NOT REMOVE THE MARKER
            sAZTH->GetAZTHPlayer(player)->changeDimension(DIMENSION_NORMAL, false, true);
        } else if (phaseDimension == DIMENSION_60 && mEntry->Expansion() > 0) {
            // CLASSIC EXPANSION CHECK
            player->TeleportTo(AzthSharedDef::blackMarket);
            ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_MULTIDIMENSION_CLASSIC_EXPCHECK, player));
        } else if (phaseDimension == DIMENSION_70 && mEntry->Expansion() > 1) {
            // TBC EXPANSION CHECK
            player->TeleportTo(AzthSharedDef::blackMarket);
            ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_MULTIDIMENSION_TBC_EXPCHECK, player));
        } else if (phaseDimension == DIMENSION_GM && player->GetSession()->GetSecurity() > SEC_PLAYER) {
            // GM security check
            player->TeleportTo(AzthSharedDef::blackMarket);
            sAZTH->GetAZTHPlayer(player)->changeDimension(DIMENSION_NORMAL);
            ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_MULTIDIMENSION_GM_ACCESSCHECK, player));
        } else if (phaseDimension == DIMENSION_GUILD && !player->GetGuild()) {
            // GUILD security check
            player->TeleportTo(AzthSharedDef::blackMarket);
            sAZTH->GetAZTHPlayer(player)->changeDimension(DIMENSION_NORMAL);
            ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_MULTIDIMENSION_GUILD_ACCESSCHECK, player));
        } else {
            // integrity check: re-enable temporary disabled dimensions or just fix possible exploits
            if (curDimension != aurDimension || curDimension != phaseDimension) {
                sAZTH->GetAZTHPlayer(player)->changeDimension(curDimension);
            }
        }
    }
    
};

void AddSC_PhaseDimensions()
{
    new PhaseDimensions();
    new PhaseDimensionsNPC();
}
