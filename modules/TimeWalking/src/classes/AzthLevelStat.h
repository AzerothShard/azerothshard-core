#ifndef AZTH_LEVEL_STATS_H
#define	AZTH_LEVEL_STATS_H

#include "Common.h"
#include "Define.h"
#include "Config.h"

enum aura_timewalking_enum
{
    // special from official dbc
    // TIMEWALKING_AURA_MOD_PARRY_RETAIL = 21352, // Decrease Parry -1
    // our from database
    TIMEWALKING_AURA_MOD_DODGE_PCT = 909080,
    TIMEWALKING_AURA_MOD_PARRY_PCT = 909081,
    TIMEWALKING_AURA_MOD_BLOCK_PCT = 909082,
    TIMEWALKING_AURA_MOD_CRIT_PCT = 909083,
    TIMEWALKING_AURA_MOD_ARMOR_PENET_PCT = 909084,
    TIMEWALKING_AURA_MOD_ALL_DAMAGE_PCT   = 909085, // db version of TIMEWALKING_AURA_MOD_DAMAGESPELL
    
    // base stats
    TIMEWALKING_AURA_MOD_STR_PCT = 909070,
    TIMEWALKING_AURA_MOD_STA_PCT = 909071,
    TIMEWALKING_AURA_MOD_AGI_PCT = 909072,
    TIMEWALKING_AURA_MOD_INT_PCT = 909073,
    TIMEWALKING_AURA_MOD_SPI_PCT = 909074,
    // our from dbc
    TIMEWALKING_AURA_MOD_INCREASE_HEALTH_PCT = 909090,
    TIMEWALKING_AURA_MOD_RESISTANCE_PCT = 909091,
    TIMEWALKING_AURA_MOD_HEALING = 909092,
    TIMEWALKING_AURA_MOD_DAMAGESPELL = 909093,
    TIMEWALKING_AURA_MOD_POWER_COST_SCHOOL_PCT = 909094,
    TIMEWALKING_AURA_MOD_STAT_PCT = 909095,
    TIMEWALKING_AURA_VISIBLE = 909096,
    // other
    TIMEWALKING_AURA_AUTO_LEVEL_SCALING = 909100,
    
    TIMEWALKING_AURA_MIN=TIMEWALKING_AURA_MOD_DODGE_PCT,
    TIMEWALKING_AURA_MAX=TIMEWALKING_AURA_AUTO_LEVEL_SCALING
};

enum TWSpecialLevels
{
    TIMEWALKING_SPECIAL_LVL_MIN=300,
    // 300 < lvl < 400 -> special levels == max_level
    // raids
    TIMEWALKING_SPECIAL_LVL_MAX_START=TIMEWALKING_SPECIAL_LVL_MIN,
    TIMEWALKING_LVL_NAXX = 381,
    TIMEWALKING_LVL_OBSIDIAN = 382,
    TIMEWALKING_LVL_THE_EYE = 383,
    TIMEWALKING_LVL_ULDUAR = 384,
    // ALGALON 385 ?
    TIMEWALKING_LVL_TOGC = 386,
    TIMEWALKING_SPECIAL_LVL_MAX_END=399,
    // 400 <= lvl <= 500 : special levels <= max_level
    // other
    TIMEWALKING_LVL_AUTO=400,
    TIMEWALKING_SPECIAL_LVL_MAX=555,
};

class AzthLevelStat
{
    friend class ACE_Singleton<AzthLevelStat, ACE_Null_Mutex>;
public:
    //GETTERS 
    uint32 GetLevel() const;
    uint32 GetRace() const;
    uint32 GetClass() const;
    uint32 GetPct(aura_timewalking_enum aura);

    //SETTERS 
    void SetLevel(uint32 level);
    void SetRace(uint32 race);
    void SetClass(uint32 Class);
    void SetPct(aura_timewalking_enum aura, uint32 val);

    AzthLevelStat(uint32 level=80, uint32 race=0, uint32 Class=0, std::map<aura_timewalking_enum, uint32> pctMap = std::map<aura_timewalking_enum, uint32>());

    std::map<aura_timewalking_enum, uint32> pctMap;
private:
    uint32 level;
    uint32 race;
    uint32 Class;
};

class AzthLevelStatMgr {
public:
    const map<uint32, AzthLevelStat> & GetLevelStatList();
    void SetLevelStatList(map<uint32, AzthLevelStat> levelStatList);
private:
    map<uint32, AzthLevelStat> levelStatList;
};

#define sAzthLevelStatMgr ACE_Singleton<AzthLevelStatMgr, ACE_Null_Mutex>::instance()

#endif