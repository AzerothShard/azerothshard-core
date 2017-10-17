#include "AzthPlayer.h"
#include "Player.h"
#include "AzthLevelStat.h"
#include "Pet.h"

uint32 AzthPlayer::GetTimeWalkingLevel() const
{
    return timeWalkingLevel;
}

void AzthPlayer::SetTimeWalkingLevel(uint32 itsTimeWalkingLevel, bool giveLevel)
{
    timeWalkingLevel = itsTimeWalkingLevel;
 
    Player* player = this->player;
    //apply debuf/buff section (spell) and enable timewalking mode
    if (itsTimeWalkingLevel > 0)
    {
        std::map<uint32, AzthLevelStat>::const_iterator itr = sAzthLevelStatMgr->GetLevelStatList().find(timeWalkingLevel * 10000 + player->getRace() * 100 + player->getClass());
        
        if (itr == sAzthLevelStatMgr->GetLevelStatList().end())
            return;
        
        AzthLevelStat stats = itr->second;

        if (player->getLevel() != timeWalkingLevel || giveLevel)
            player->GiveLevel(timeWalkingLevel);

        player->SetUInt32Value(PLAYER_XP, 0);
        player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
        player->SetAuraStack(TIMEWALKING_AURA_MOD_HEALING, player, stats.GetHealPct());
        player->SetAuraStack(TIMEWALKING_AURA_MOD_DAMAGESPELL, player, stats.GetDamPct());
        player->AddAura(TIMEWALKING_AURA_VISIBLE, player);
        QueryResult timewalkingCharactersActive_table = CharacterDatabase.PQuery(("INSERT IGNORE INTO azth_timewalking_characters_active (`id`, `level`) VALUES ('%d', '%d');"), player->GetGUID(), player->azthPlayer->GetTimeWalkingLevel());
    }
    else
    {

        if (player->getLevel() != timeWalkingLevel || giveLevel) {
            player->GiveLevel(sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL));

            if (player->GetPet()) {
                player->GetPet()->GivePetLevel(sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL));
            }
        }

        player->SetUInt32Value(PLAYER_XP, 0);
        player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
        player->RemoveAura(TIMEWALKING_AURA_MOD_HEALING);
        player->RemoveAura(TIMEWALKING_AURA_MOD_DAMAGESPELL);
        player->RemoveAura(TIMEWALKING_AURA_VISIBLE);

        QueryResult timewalkingCharactersActive_table = CharacterDatabase.PQuery(("DELETE FROM azth_timewalking_characters_active WHERE  `id`=%d;"), player->GetGUID());
    }
}
