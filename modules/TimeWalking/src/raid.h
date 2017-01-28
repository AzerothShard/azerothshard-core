#ifndef AZTH_RAID_H
#define	AZTH_RAID_H

#include "Common.h"
#include "Define.h"
#include "Config.h"

class raid
{
    friend class ACE_Singleton<raid, ACE_Null_Mutex>;
public:
	//GETTERS
	uint32 GetId() const;
	string GetName() const;
    uint32 GetExp() const;
	uint32 GetPhase() const;
    uint32 GetLevel() const;
    uint32 GetBonus() const;
    uint32 GetCriteria() const;

	//SETTERS
	void SetId(uint32 id);
	void SetName(string name);
    void SetExp(uint32 exp);
	void SetPhase(uint32 phase);
    void SetLevel(uint32 level);
    void SetBonus(uint32 bonus);
    void SetCriteria(uint32 criteria);

	raid();
	raid(uint32 id, string name, uint32 exp, uint32 phase, uint32 level, uint32 bonus, uint32 criteria);

    map<uint32, raid> GetRaidList() const;
    void SetRaidList(map<uint32, raid> raidList);

private:
	uint32 id;
	string name;
    uint32 exp;
	uint32 phase;
    uint32 level;
    uint32 bonus;
    uint32 criteria;
    map<uint32, raid> raidList;
};

#define sAzthRaid ACE_Singleton<raid, ACE_Null_Mutex>::instance()

#endif AZTH_RAID_H