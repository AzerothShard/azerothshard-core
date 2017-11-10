#include "AzthPlayer.h"
#include "Player.h"
#include "AzthLevelStat.h"
#include "AzthUtils.h"
#include "Pet.h"

uint32 AzthPlayer::GetTimeWalkingLevel() const
{
    return timeWalkingLevel;
}

bool AzthPlayer::isTimeWalking(bool skipSpecial) const
{
    if (skipSpecial && timeWalkingLevel>0 && ( timeWalkingLevel <= sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) || timeWalkingLevel == TIMEWALKING_LVL_AUTO )) {
        // it includes TIMEWALKING_LEVEL_AUTO
        return true;
    } else if (!skipSpecial && timeWalkingLevel>0) {
        return true;
    }
    
    return false;
}

uint32 AzthPlayer::getTwItemLevel(uint32 twLevel) {       
    switch (twLevel) {
        case TIMEWALKING_LVL_NAXX:
        case TIMEWALKING_LVL_OBSIDIAN:
        case TIMEWALKING_LVL_THE_EYE:
            return 213;
        break;
        case TIMEWALKING_LVL_ULDUAR:
        case TIMEWALKING_LVL_TOGC:
            return 232;
        break;
    }
    
    return 0;
}

void AzthPlayer::SetTimeWalkingLevel(uint32 itsTimeWalkingLevel, bool clearAuras/*=true*/, bool save /*=true*/, bool login/*=false*/)
{
    std::string lvlName=sAzthUtils->getLevelInfo(itsTimeWalkingLevel);
    
    if (!login) {
        // hacking attempt?
        if (timeWalkingLevel==0 && player->getLevel()<sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
            return;
        
        uint32 iLvl=player->azthPlayer->getTwItemLevel(itsTimeWalkingLevel);
        if (iLvl && !player->azthPlayer->checkItems(iLvl)) {
            ChatHandler(player->GetSession()).PSendSysMessage("Questa modalità ha l'item level massimo: %u",iLvl);
            return;
        }
    
        // we should skip invalid levels and use available specials (>300) if needed
        if (itsTimeWalkingLevel >= sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) && lvlName.compare(std::to_string(itsTimeWalkingLevel)) == 0 /*equals*/)
            return;
    }
    
    uint32 realLevel = itsTimeWalkingLevel;
    uint32 statsLevel = itsTimeWalkingLevel;
    
    if (itsTimeWalkingLevel>TIMEWALKING_SPECIAL_LVL_MIN && itsTimeWalkingLevel<=TIMEWALKING_SPECIAL_LVL_MAX) { // 300 + 255 levels
        
        if (itsTimeWalkingLevel == TIMEWALKING_LVL_AUTO) {
            WorldLocation pos = WorldLocation(player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());
            
            uint32 posLvl=sAzthUtils->getPositionLevel(false, player->GetMap(), pos);
            realLevel = statsLevel = posLvl ? posLvl : player->getLevel();
            
            switch(player->azthPlayer->getCurrentDimensionByAura()) {
                case DIMENSION_60:
                    if (player->getLevel() > 60)
                        realLevel = 60;
                break;
                case DIMENSION_70:
                    if (player->getLevel() > 70)
                        realLevel = 70;
                break;
            }
            
        } else {
            realLevel=sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
        }
    }

    Player* player = this->player;
    //apply debuf/buff section (spell) and enable timewalking mode
    if (itsTimeWalkingLevel > 0)
    { 
        if (player->HasAura(TIMEWALKING_AURA_VISIBLE)) {
            // we must remove before set again
            SetTimeWalkingLevel(0, false, false, login);
        }
        
        std::map<uint32, AzthLevelStat>::const_iterator itr = sAzthLevelStatMgr->GetLevelStatList().find(statsLevel * 10000 + player->getRace() * 100 + player->getClass());
        
        if (itr == sAzthLevelStatMgr->GetLevelStatList().end())
            return;
        
        AzthLevelStat stats = itr->second;
        
        //set must be before givelevel allowing make the check inside givelevel to avoid level mail
        timeWalkingLevel = itsTimeWalkingLevel;

        if (player->getLevel() != realLevel)
            player->GiveLevel(realLevel);

        player->SetUInt32Value(PLAYER_XP, 0);
        player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
        
        for ( auto& a : stats.pctMap) {
            if (a.first != TIMEWALKING_AURA_VISIBLE)
                player->SetAuraStack(a.first, player, a.second);
        }

        player->AddAura(TIMEWALKING_AURA_VISIBLE, player);
        if (save) {
            QueryResult timewalkingCharactersActive_table = CharacterDatabase.PQuery(("INSERT IGNORE INTO azth_timewalking_characters_active (`id`, `level`) VALUES ('%d', '%d');"), player->GetGUID(), player->azthPlayer->GetTimeWalkingLevel());
        }
        
        ChatHandler(player->GetSession()).PSendSysMessage("Sei in modalità |cffff0000 Timewalking livello: %s|r", lvlName.c_str());
    }
    else
    {
        // used just to be able to loop on keys to remove auras
        std::map<uint32, AzthLevelStat>::const_iterator itr = sAzthLevelStatMgr->GetLevelStatList().find(player->getLevel() * 10000 + player->getRace() * 100 + player->getClass());
        
        if (itr == sAzthLevelStatMgr->GetLevelStatList().end())
            return;
        
        AzthLevelStat stats = itr->second;
        
        if (save) {
            player->GiveLevel(sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL));

            if (player->GetPet()) {
                player->GetPet()->GivePetLevel(sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL));
            }
        }

        player->SetUInt32Value(PLAYER_XP, 0);
        player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN); 
        
        for ( auto a : stats.pctMap) {
            player->RemoveAura(a.first);
        }

        player->RemoveAura(TIMEWALKING_AURA_VISIBLE);

        // reset must be after givelevel allowing make the check inside givelevel to avoid level mail
        timeWalkingLevel = 0;

        if (save) {
            QueryResult timewalkingCharactersActive_table = CharacterDatabase.PQuery(("DELETE FROM azth_timewalking_characters_active WHERE  `id`=%d;"), player->GetGUID());
            // we announce it only when not login, because TW could be removed temporary (on level switch or on login for example)
            ChatHandler(player->GetSession()).PSendSysMessage("Sei uscito dalla modalità |cffff0000 Timewalking!|r");
        }
    }
    
    sAzthUtils->updateTwLevel(player, player->GetGroup());
    
    if (clearAuras) {
        sAzthUtils->removeTimewalkingAura(player);
        if (player->GetPet() != NULL) {
            sAzthUtils->removeTimewalkingAura(player->GetPet());
        }
    }
    
    /*if (save) {
        player->SaveToDB(false, false);
    }*/
}
