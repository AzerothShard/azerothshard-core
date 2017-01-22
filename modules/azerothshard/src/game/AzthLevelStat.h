#include "Common.h"
#include "Define.h"
#include "Config.h"

enum aura_timewalking_enum
{
    TIMEWALKING_AURA_MOD_HEALTH = 909090,
    TIMEWALKING_AURA_MOD_RESISTANCE = 909091,
    TIMEWALKING_AURA_MOD_HEALING = 909092,
    TIMEWALKING_AURA_MOD_DAMAGE = 909093,
    TIMEWALKING_AURA_MOD_POWERCOST = 909094,
    TIMEWALKING_AURA_MOD_ALLSTATS = 909095,
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
    uint32 GetHealth() const;
    uint32 GetResistance() const;
    uint32 GetHealing() const;
    uint32 GetDamage() const;
    uint32 GetPowerCost() const;
    uint32 GetAllStats() const;
    float GetMana() const;
    uint32 GetCritChance() const;
    uint32 GetMissChance() const;
    uint32 GetDodgeChance() const;
    uint32 GetParryChance() const;
    uint32 GetBlockChance() const;

    map<uint32, AzthLevelStat> GetLevelStatList() const;


    //SETTERS 
    void SetLevel(uint32 level);
    void SetRace(uint32 race);
    void SetClass(uint32 Class);
    void SetHealth(uint32 health);
    void SetResistance(uint32 resistance);
    void SetHealing(uint32 healing);
    void SetDamage(uint32 damage);
    void SetPowerCost(uint32 powerCost);
    void SetAllStats(uint32 allStas);
    void SetMana(float mana);
    void SetCritChance(uint32 critChance);
    void SetMissChance(uint32 missChance);
    void SetDodgeChance(uint32 dodgeChance);
    void SetParryChance(uint32 parryChance);
    void SetBlockChance(uint32 blockChance);

    void SetLevelStatList(map<uint32, AzthLevelStat> levelStatList);

    AzthLevelStat();
    AzthLevelStat(uint32 level, uint32 race, uint32 Class, uint32 health, uint32 resistance, uint32 healing, uint32 damage, uint32 powerCost, uint32 allStats, float mana, uint32 critChance, uint32 missChance, uint32 dodgeChance, uint32 parryChance, uint32 blockChance);


private:
    uint32 level;
    uint32 race;
    uint32 Class;
    uint32 health;
    uint32 resistance;
    uint32 healing;
    uint32 damage;
    uint32 powerCost;
    uint32 allStats;
    float mana;
    uint32 critChance;
    uint32 missChance;
    uint32 dodgeChance;
    uint32 parryChance;
    uint32 blockChance;
    map<uint32, AzthLevelStat> levelStatList;
};

#define sAzthLevelStat ACE_Singleton<AzthLevelStat, ACE_Null_Mutex>::instance()