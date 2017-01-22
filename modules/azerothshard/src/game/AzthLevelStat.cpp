#include "AzthLevelStat.h"

AzthLevelStat::AzthLevelStat()
{
    level = uint32(80);
    health = uint32(0); 
    resistance = uint32(0); 
    healing = uint32(0);
    damage = uint32(0);
    powerCost = uint32(0);
    allStats = uint32(0);
    mana = float(0.0f);
    critChance = uint32(0);
    missChance = uint32(0);
    dodgeChance = uint32(0);
    parryChance = uint32(0);
    blockChance = uint32(0);
}

AzthLevelStat::AzthLevelStat(uint32 level, uint32 race, uint32 Class, uint32 health, uint32 resistance, uint32 healing, uint32 damage, uint32 powerCost, uint32 allStats, float mana, uint32 critChance, uint32 missChance, uint32 dodgeChance, uint32 parryChance, uint32 blockChance)
{
    this->level = level;
    this->race = race;
    this->Class = Class;
    this->health = health;
    this->resistance = resistance;
    this->healing = healing;
    this->damage = damage;
    this->powerCost = powerCost;
    this->allStats = allStats;
    this->mana = mana;
    this->critChance = critChance;
    this->missChance = missChance;
    this->dodgeChance = dodgeChance;
    this->parryChance = parryChance;
    this->blockChance = blockChance;
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

uint32 AzthLevelStat::GetHealth() const
{
    return health;
}

void AzthLevelStat::SetHealth(uint32 health)
{
    this->health = health;
}

uint32 AzthLevelStat::GetResistance() const
{
    return resistance;
}

void AzthLevelStat::SetResistance(uint32 resistance)
{
    this->resistance = resistance;
}

uint32 AzthLevelStat::GetHealing() const
{
    return healing;
}

void AzthLevelStat::SetHealing(uint32 healing)
{
    this->healing = healing;
}

uint32 AzthLevelStat::GetDamage() const
{
    return damage;
}

void AzthLevelStat::SetDamage(uint32 damage)
{
    this->damage = damage;
}

uint32 AzthLevelStat::GetPowerCost() const
{
    return powerCost;
}

void AzthLevelStat::SetPowerCost(uint32 powerCost)
{
    this->powerCost = powerCost;
}

uint32 AzthLevelStat::GetAllStats() const
{
    return allStats;
}

void AzthLevelStat::SetAllStats(uint32 allStats)
{
    this->allStats = allStats;
}

float AzthLevelStat::GetMana() const
{
    return mana;
}

void AzthLevelStat::SetMana(float mana)
{
    this->mana = mana;
}

uint32 AzthLevelStat::GetCritChance() const
{
    return critChance;
}

void AzthLevelStat::SetCritChance(uint32 critChance)
{
    this->critChance = critChance;
}

uint32 AzthLevelStat::GetMissChance() const
{
    return missChance;
}

void AzthLevelStat::SetMissChance(uint32 missChance)
{
    this->missChance = missChance;
}

uint32 AzthLevelStat::GetDodgeChance() const
{
    return dodgeChance;
}

void AzthLevelStat::SetDodgeChance(uint32 dodgeChance)
{
    this->dodgeChance = dodgeChance;
}

uint32 AzthLevelStat::GetParryChance() const
{
    return parryChance;
}

void AzthLevelStat::SetParryChance(uint32 parryChance)
{
    this->parryChance = parryChance;
}

uint32 AzthLevelStat::GetBlockChance() const
{
    return blockChance;
}

void AzthLevelStat::SetBlockChance(uint32 blockChance)
{
    this->blockChance = blockChance;
}