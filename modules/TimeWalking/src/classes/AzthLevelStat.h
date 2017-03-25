#ifndef AZTH_LEVEL_STATS_H
#define	AZTH_LEVEL_STATS_H

#include "Common.h"
#include "Define.h"
#include "Config.h"

enum aura_timewalking_enum
{
    TIMEWALKING_AURA_MOD_HEALING = 909092,
    TIMEWALKING_AURA_MOD_DAMAGESPELL = 909093,
    TIMEWALKING_AURA_VISIBLE = 909096
};

class AzthLevelStat
{
    friend class ACE_Singleton<AzthLevelStat, ACE_Null_Mutex>;
public:
    //GETTERS 
    uint32 GetLevel() const;
    uint32 GetRace() const;
    uint32 GetClass() const;
    uint32 GetStrPct() const;
    uint32 GetAgiPct() const;
    uint32 GetStaPct() const;
    uint32 GetIntPct() const;
    uint32 GetSpiPct() const;
    uint32 GetDamPct() const;
    uint32 GetHealPct() const;

    map<uint32, AzthLevelStat> GetLevelStatList() const;


    //SETTERS 
    void SetLevel(uint32 level);
    void SetRace(uint32 race);
    void SetClass(uint32 Class);
    void SetStrPct(uint32 strength);
    void SetAgiPct(uint32 agility);
    void SetStaPct(uint32 stamina);
    void SetIntPct(uint32 intellect);
    void SetSpiPct(uint32 spirit);
    void SetDamPct(uint32 damage);
    void SetHealPct(uint32 heal);


    void SetLevelStatList(map<uint32, AzthLevelStat> levelStatList);

    AzthLevelStat();
    AzthLevelStat(uint32 level, uint32 race, uint32 Class, uint32 strength, uint32 agility, uint32 stamina, uint32 intellect, uint32 spirit, uint32 damage, uint32 heal);


private:
    uint32 level;
    uint32 race;
    uint32 Class;
    uint32 strength;
    uint32 agility;
    uint32 stamina;
    uint32 intellect;
    uint32 spirit;
    uint32 damage;
    uint32 heal;
    map<uint32, AzthLevelStat> levelStatList;
};

#define sAzthLevelStat ACE_Singleton<AzthLevelStat, ACE_Null_Mutex>::instance()

#endif