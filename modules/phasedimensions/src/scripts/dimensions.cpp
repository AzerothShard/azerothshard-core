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
#include "StringFormat.h"
#include "AccountMgr.h"

class PhaseDimensionsNPC : public CreatureScript
{
public:
    PhaseDimensionsNPC() : CreatureScript("PhaseDimensionsNPC") {}


    bool OnGossipHello(Player* player, Creature* creature) override
    {
        uint32 currDimension = sAZTH->GetAZTHPlayer(player)->getCurrentDimensionByMark();
        std::string dimensionCurrent = ACORE::StringFormat("%s%s", sAzthLang->get(AZTH_LANG_DIMENSION_CURRENT, player), sAzthUtils->getDimensionName(currDimension).c_str());

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, dimensionCurrent, GOSSIP_SENDER_MAIN, 0);

        if (CanShowDimension(player, DIMENSION_NORMAL))
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, sAzthLang->get(AZTH_LANG_DIMENSION_ILLUSORY, player), GOSSIP_SENDER_MAIN, DIMENSION_NORMAL);

        if (CanShowDimension(player, DIMENSION_GUILD))
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, sAzthLang->get(AZTH_LANG_DIMENSION_GUILD, player), GOSSIP_SENDER_MAIN, DIMENSION_GUILD);

        if (CanShowDimension(player, DIMENSION_PVP))
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, sAzthLang->get(AZTH_LANG_DIMENSION_PVP, player), GOSSIP_SENDER_MAIN, DIMENSION_PVP);

        if (CanShowDimension(player, DIMENSION_ENTERTAINMENT))
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, sAzthLang->get(AZTH_LANG_DIMENSION_ENTERTAIN, player), GOSSIP_SENDER_MAIN, DIMENSION_ENTERTAINMENT);

        if (CanShowDimension(player, DIMENSION_RPG))
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, sAzthLang->get(AZTH_LANG_DIMENSION_GDR, player), GOSSIP_SENDER_MAIN, DIMENSION_RPG);

        if (CanShowDimension(player, DIMENSION_60))
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, sAzthLang->get(AZTH_LANG_DIMENSION_LVL60, player), GOSSIP_SENDER_MAIN, DIMENSION_60);

        if (CanShowDimension(player, DIMENSION_70))
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, sAzthLang->get(AZTH_LANG_DIMENSION_LVL70, player), GOSSIP_SENDER_MAIN, DIMENSION_70);
        
        if (CanShowDimension(player, DIMENSION_TEST))
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, sAzthLang->get(AZTH_LANG_DIMENSION_TEST, player), GOSSIP_SENDER_MAIN, DIMENSION_TEST);
        
        if (CanShowDimension(player, DIMENSION_GM))
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, sAzthLang->get(AZTH_LANG_DIMENSION_GM, player), GOSSIP_SENDER_MAIN, DIMENSION_GM);

        SendGossipMenuFor(player, 1, creature);
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 sender, uint32 action) override
    {
        CloseGossipMenuFor(player);
        
        if (!action)
            return true;
        
        if (sender == GOSSIP_SENDER_MAIN)
            sAZTH->GetAZTHPlayer(player)->changeDimension(action, true);

        return true;
    }

private:
    bool CanShowDimension(Player* player, PhaseDimensionsEnum dim)
    {
        uint32 accountLevel = player->GetSession()->GetSecurity();
        uint32 currDimension = sAZTH->GetAZTHPlayer(player)->getCurrentDimensionByMark();
        bool sameDim = false;
        bool accesLevel = false;
        uint32 minLevel;

        switch (dim)
        {
        case DIMENSION_NORMAL:
            minLevel = sConfigMgr->GetIntDefault("Azth.NPC.Dimension.AccountSec.Min.Normal", 4);
            break;
        case DIMENSION_GUILD:
            minLevel = sConfigMgr->GetIntDefault("Azth.NPC.Dimension.AccountSec.Min.Guild", 4);
            break;
        case DIMENSION_PVP:
            minLevel = sConfigMgr->GetIntDefault("Azth.NPC.Dimension.AccountSec.Min.PvP", 4);
            break;
        case DIMENSION_ENTERTAINMENT:
            minLevel = sConfigMgr->GetIntDefault("Azth.NPC.Dimension.AccountSec.Min.Entertainment", 4);
            break;
        case DIMENSION_RPG:
            minLevel = sConfigMgr->GetIntDefault("Azth.NPC.Dimension.AccountSec.Min.RPG", 4);
            break;
        case DIMENSION_60:
            minLevel = sConfigMgr->GetIntDefault("Azth.NPC.Dimension.AccountSec.Min.60", 4);
            break;
        case DIMENSION_70:
            minLevel = sConfigMgr->GetIntDefault("Azth.NPC.Dimension.AccountSec.Min.70", 4);
            break;
        case DIMENSION_TEST:
            minLevel = sConfigMgr->GetIntDefault("Azth.NPC.Dimension.AccountSec.Min.Test", 4);
            break;
        case DIMENSION_GM:
            minLevel = sConfigMgr->GetIntDefault("Azth.NPC.Dimension.AccountSec.Min.GM", 4);
            break;
        default:
            ASSERT(false);
            break;
        }

        if (currDimension == (uint32)dim)
            sameDim = true;

        if (accountLevel >= minLevel)
            accesLevel = true;

        // For DIMENSION_NORMAL need non PvP account
        if (dim == DIMENSION_NORMAL && (sAZTH->GetAZTHPlayer(player)->isPvP() || sameDim || !accesLevel))
            return false;

        // For DIMENSION_GM need min level account (force) 1 (non player)
        if (dim == DIMENSION_GM && AccountMgr::IsPlayerAccount((uint32)player->GetSession()->GetSecurity()))
            return false;

        if (!sameDim && accesLevel)
            return true;

        return false;
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

        Player* _target = target->ToPlayer();

        if (player->GetGUIDLow() == _target->GetGUIDLow())
            return true; // strange case but could happen

        uint32 playerPhaseDimension = sAZTH->GetAZTHPlayer(player)->getCurrentDimensionByPhase();
        uint32 targetPhaseDimension = sAZTH->GetAZTHPlayer(_target)->getCurrentDimensionByPhase();
        
        if (targetPhaseDimension == playerPhaseDimension)
            return true;

        if (targetPhaseDimension == DIMENSION_NORMAL || player->IsGameMaster() && !sAZTH->GetAZTHPlayer(player)->changeDimension(targetPhaseDimension, true))
            return false;
        else
        {
            std::string dimSource = sAzthUtils->getDimensionName(playerPhaseDimension);
            std::string dimDest = sAzthUtils->getDimensionName(targetPhaseDimension);

            ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_MULTIDIMENSION_CANNOT_BE_SUMMONED, player));
            ChatHandler(_target->GetSession()).SendSysMessage(sAzthLang->getf(AZTH_LANG_MULTIDIMENSION_CANNOT_BE_SUMMONED_SUMMONER, player, dimSource.c_str(), dimDest.c_str()));
            return false;
        }

        return true;
    }

    void OnMapChanged(Player* player) override
    { 
        _zoneCheck(player, player->GetMapId(), player->GetZoneId(), player->GetAreaId());
    }
    
    void OnUpdateZone(Player *player, uint32 zone, uint32 area) override
    {
        AreaTableEntry const* aEntry = sAreaTableStore.LookupEntry(area);
        if (!aEntry)
            return;

        _zoneCheck(player, aEntry->mapid , zone, area);
    }    
    
    void _zoneCheck(Player *player, uint32 mapid, uint32 zoneid, uint32 areaid)
    {
        if (!player)
            return;
        
        uint32 curDimension   = sAZTH->GetAZTHPlayer(player)->getCurrentDimensionByMark();
        uint32 aurDimension   = sAZTH->GetAZTHPlayer(player)->getCurrentDimensionByAura();

        // we should make an integrity check with phase
        // avoiding cases where we're on a special dimension
        // without aura (hacking/exploit)
        uint32 phaseDimension = sAZTH->GetAZTHPlayer(player)->getCurrentDimensionByPhase();
        
        MapEntry const* mEntry = sMapStore.LookupEntry(mapid);
        
        if (sAzthUtils->isPhasedDimension(curDimension) && sAzthUtils->isSharedArea(player, mEntry, zoneid, areaid))
        {
            // TEMPORARY DISABLE DIMENSION IN BG BUT DO NOT REMOVE THE MARKER
            sAZTH->GetAZTHPlayer(player)->changeDimension(DIMENSION_NORMAL, false, true);
        }
        else if (phaseDimension == DIMENSION_60 && mEntry->Expansion() > 0)
        {
            // CLASSIC EXPANSION CHECK
            player->TeleportTo(AzthSharedDef::blackMarket);
            ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_MULTIDIMENSION_CLASSIC_EXPCHECK, player));
        }
        else if (phaseDimension == DIMENSION_70 && mEntry->Expansion() > 1)
        {
            // TBC EXPANSION CHECK
            player->TeleportTo(AzthSharedDef::blackMarket);
            ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_MULTIDIMENSION_TBC_EXPCHECK, player));
        }
        else if (phaseDimension == DIMENSION_GM && player->GetSession()->GetSecurity() > SEC_PLAYER)
        {
            // GM security check
            player->TeleportTo(AzthSharedDef::blackMarket);
            sAZTH->GetAZTHPlayer(player)->changeDimension(DIMENSION_NORMAL);
            ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_MULTIDIMENSION_GM_ACCESSCHECK, player));
        }
        else if (phaseDimension == DIMENSION_GUILD && !player->GetGuild())
        {
            // GUILD security check
            player->TeleportTo(AzthSharedDef::blackMarket);
            sAZTH->GetAZTHPlayer(player)->changeDimension(DIMENSION_NORMAL);
            ChatHandler(player->GetSession()).SendSysMessage(sAzthLang->get(AZTH_LANG_MULTIDIMENSION_GUILD_ACCESSCHECK, player));
        }
        else
        {
            // integrity check: re-enable temporary disabled dimensions or just fix possible exploits
            if (curDimension != aurDimension || curDimension != phaseDimension)
                sAZTH->GetAZTHPlayer(player)->changeDimension(curDimension);
        }
    }    
};

void AddSC_PhaseDimensions()
{
    new PhaseDimensions();
    new PhaseDimensionsNPC();
}
