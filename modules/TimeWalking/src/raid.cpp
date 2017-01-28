#include "raid.h"

raid::raid()
{
	id = uint32(0);
	name = string("");
    exp = uint32(0);
	phase = uint32(0);
    level = uint32(0);
    bonus = uint32(0);
    criteria = uint32(0);
}

raid::raid(uint32 ItsId, string ItsName, uint32 ItsExp, uint32 ItsPhase, uint32 ItsLevel, uint32 ItsBonus, uint32 ItsCriteria)
{
	id = ItsId;
	name = ItsName;
    exp = ItsExp;
	phase = ItsPhase;
    level = ItsLevel;
    bonus = ItsBonus;
    criteria = ItsCriteria;
}

map<uint32, raid> raid::GetRaidList() const
{
    return raidList;
}

void raid::SetRaidList(map<uint32, raid> raidList)
{
    this->raidList = raidList;
}

uint32 raid::GetId() const
{
	return id;
}

void raid::SetId(uint32 ItsId)
{
	id = ItsId;
}

string raid::GetName() const
{
	return name;
}

void raid::SetName(string ItsName)
{
	name = ItsName;
}

uint32 raid::GetExp() const
{
    return exp;
}

void raid::SetExp(uint32 ItsExp)
{
    exp = ItsExp;
}

uint32 raid::GetPhase() const
{
	return phase;
}

void raid::SetPhase(uint32 ItsPhase)
{
	phase = ItsPhase;
}

uint32 raid::GetLevel() const
{
    return level;
}

void raid::SetLevel(uint32 ItsLevel)
{
    level = ItsLevel;
}

uint32 raid::GetBonus() const
{
    return bonus;
}

void raid::SetBonus(uint32 ItsBonus)
{
    bonus = ItsBonus;
}

uint32 raid::GetCriteria() const
{
    return criteria;
}

void raid::SetCriteria(uint32 ItsCriteria)
{
    criteria = ItsCriteria;
}