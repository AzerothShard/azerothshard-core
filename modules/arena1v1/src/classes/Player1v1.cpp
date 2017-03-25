#include "AzthPlayer.h"
#include "Player.h"

uint32 AzthPlayer::getArena1v1Info(uint8 type) { return arena1v1Info[type]; }

void AzthPlayer::setArena1v1Info(uint8 type, uint32 value) {
  arena1v1Info[type] = value;
}
