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
    if (skipSpecial && timeWalkingLevel>0 && timeWalkingLevel<sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)) {
        return true;
    } else if (!skipSpecial && timeWalkingLevel>0) {
        return true;
    }
    
    return false;
}

void AzthPlayer::SetTimeWalkingLevel(uint32 itsTimeWalkingLevel, bool giveLevel)
{
    if (itsTimeWalkingLevel == sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)) // we should skip level 80 and use specials (>300) if needed
        return;
    
    uint32 realLevel = itsTimeWalkingLevel;
    
    if (itsTimeWalkingLevel>TIMEWALKING_SPECIAL_LVL_MIN && itsTimeWalkingLevel<=TIMEWALKING_SPECIAL_LVL_MAX) { // 300 + 255 levels
        realLevel=sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
    }

    Player* player = this->player;
    //apply debuf/buff section (spell) and enable timewalking mode
    if (itsTimeWalkingLevel > 0)
    {
        std::map<uint32, AzthLevelStat>::const_iterator itr = sAzthLevelStatMgr->GetLevelStatList().find(itsTimeWalkingLevel * 10000 + player->getRace() * 100 + player->getClass());
        
        if (itr == sAzthLevelStatMgr->GetLevelStatList().end())
            return;
        
        AzthLevelStat stats = itr->second;
        
        timeWalkingLevel = itsTimeWalkingLevel;

        if (player->getLevel() != realLevel || giveLevel)
            player->GiveLevel(realLevel);

        player->SetUInt32Value(PLAYER_XP, 0);
        player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
        
        
        for ( auto& a : stats.pctMap) {
            if (a.first != TIMEWALKING_AURA_VISIBLE)
                player->SetAuraStack(a.first, player, a.second);
        }

        player->AddAura(TIMEWALKING_AURA_VISIBLE, player);
        QueryResult timewalkingCharactersActive_table = CharacterDatabase.PQuery(("INSERT IGNORE INTO azth_timewalking_characters_active (`id`, `level`) VALUES ('%d', '%d');"), player->GetGUID(), player->azthPlayer->GetTimeWalkingLevel());

        ChatHandler(player->GetSession()).PSendSysMessage("Sei entrato nella modalità |cffff0000|Timewalking!|r");
    }
    else
    {
        // used just to be able to loop on keys to remove auras
        std::map<uint32, AzthLevelStat>::const_iterator itr = sAzthLevelStatMgr->GetLevelStatList().find(timeWalkingLevel * 10000 + player->getRace() * 100 + player->getClass());
        
        if (itr == sAzthLevelStatMgr->GetLevelStatList().end())
            return;
        
        AzthLevelStat stats = itr->second;
        
        player->GiveLevel(sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL));

        if (player->GetPet()) {
            player->GetPet()->GivePetLevel(sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL));
        }

        player->SetUInt32Value(PLAYER_XP, 0);
        player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN); 
        
        for ( auto a : stats.pctMap) {
            player->RemoveAura(a.first);
        }

        player->RemoveAura(TIMEWALKING_AURA_VISIBLE);

        timeWalkingLevel = 0;
        QueryResult timewalkingCharactersActive_table = CharacterDatabase.PQuery(("DELETE FROM azth_timewalking_characters_active WHERE  `id`=%d;"), player->GetGUID());
        
        ChatHandler(player->GetSession()).PSendSysMessage("Sei uscito dalla modalità |cffff0000|Timewalking!|r");
    }
    
    sAzthUtils->updateTwLevel(player, player->GetGroup());
}
