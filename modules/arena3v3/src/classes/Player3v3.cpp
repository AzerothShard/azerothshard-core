#include "AzthPlayer.h"

uint32 AzthPlayer::getArena3v3Info(uint8 type) { return arena3v3Info[type]; }

void AzthPlayer::setArena3v3Info(uint8 type, uint32 value) {
    arena3v3Info[type] = value;
}