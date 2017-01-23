#include "AzthLevelStat.h"

AzthLevelStat::AzthLevelStat()
{
    level = uint32(80);
    race = uint32(0);
    Class = uint32(0);
    strength = uint32(0);
    agility = uint32(0);
    stamina = uint32(0);
    intellect = uint32(0);
    spirit = uint32(0);
    powerCost = uint32(0);
}

AzthLevelStat::AzthLevelStat(uint32 level, uint32 race, uint32 Class, uint32 strength, uint32 agility, uint32 stamina, uint32 intellect, uint32 spirit, uint32 powerCost)
{
    this->level = level;
    this->race = race;
    this->Class = Class;
    this->strength = strength;
    this->agility = agility;
    this->stamina = stamina;
    this->intellect = intellect;
    this->spirit = spirit;
    this->powerCost = powerCost;
}

map<uint32, AzthLevelStat> AzthLevelStat::GetLevelStatList() const
{
    return levelStatList;
}

void AzthLevelStat::SetLevelStatList(map<uint32, AzthLevelStat> levelStatList)
{
    this->levelStatList = levelStatList;
}

uint32 AzthLevelStat::GetLevel() const
{
    return level;
}

void AzthLevelStat::SetLevel(uint32 level)
{
    this->level = level;
}

uint32 AzthLevelStat::GetRace() const
{
    return race;
}

void AzthLevelStat::SetRace(uint32 race)
{
    this->race = race;
}

uint32 AzthLevelStat::GetClass() const
{
    return Class;
}

void AzthLevelStat::SetClass(uint32 Class)
{
    this->Class = Class;
}

uint32 AzthLevelStat::GetStrPct() const
{
    return strength;
}

void AzthLevelStat::SetStrPct(uint32 strength)
{
    this->strength = strength;
}

uint32 AzthLevelStat::GetAgiPct() const
{
    return agility;
}

void AzthLevelStat::SetAgiPct(uint32 agility)
{
    this->agility = agility;
}

uint32 AzthLevelStat::GetStaPct() const
{
    return stamina;
}

void AzthLevelStat::SetStaPct(uint32 stamina)
{
    this->stamina = stamina;
}

uint32 AzthLevelStat::GetIntPct() const
{
    return intellect;
}

void AzthLevelStat::SetIntPct(uint32 intellect)
{
    this->intellect = intellect;
}

uint32 AzthLevelStat::GetSpiPct() const
{
    return spirit;
}

void AzthLevelStat::SetSpiPct(uint32 spirit)
{
    this->spirit = spirit;
}

uint32 AzthLevelStat::GetPowerCost() const
{
    return powerCost;
}

void AzthLevelStat::SetPowerCost(uint32 powerCost)
{
    this->powerCost = powerCost;
}