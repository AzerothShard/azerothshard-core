#include "AzthPlayer.h"
#include "Chat.h"
#include "Player.h"

void AzthPlayer::SetPlayerQuestRate(float rate) {
    playerQuestRate = rate;
}

float AzthPlayer::GetPlayerQuestRate() {
    return playerQuestRate;
}

bool AzthPlayer::AzthSelfChangeXp(float rate) {

    //if (!player || !player->GetSession())
    //    return false;

    ChatHandler* ch = new ChatHandler(player->GetSession());

    float maxRate = sWorld->getFloatConfig(CONFIG_PLAYER_MAXIMUM_INDIVIDUAL_XP_RATE);
    if (rate < 0 || rate > maxRate) {
        ch->PSendSysMessage("|CFF7BBEF7[Custom Rates]|r: Invalid rate specified, must be in interval [0, %.2f].", maxRate);
        return false;
    }

    CustomRates::SaveXpRateToDB(player, rate);

    player->azthPlayer->SetPlayerQuestRate(rate);

    if (player->azthPlayer->GetPlayerQuestRate() == 0.0f)
        ch->PSendSysMessage("|CFF7BBEF7[Custom Rates]|r: Quest & Dungeons XP Rate set to 0. You won't gain any experience from now on.");
    else
        ch->PSendSysMessage("|CFF7BBEF7[Custom Rates]|r: Quest & Dungeons XP Rate set to %.2f.", player->azthPlayer->GetPlayerQuestRate());
    return true;
};