#include "AzthPlayer.h"

void AzthPlayer::SetTempGear(bool PVPTempGear)
{
    this->PVPTempGear = PVPTempGear;
}

bool AzthPlayer::hasGear()
{
    return PVPTempGear;
}

