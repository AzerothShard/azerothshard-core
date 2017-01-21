#include "Common.h"

class raid
{
public:
	//GETTERS
	uint32 GetId() const;
	string GetName() const;
    uint32 GetExp() const;
	uint32 GetPhase() const;
    uint32 GetLevel() const;
    uint32 GetBonus() const;

	//SETTERS
	void SetId(uint32 id);
	void SetName(string name);
    void SetExp(uint32 exp);
	void SetPhase(uint32 phase);
    void SetLevel(uint32 level);
    void SetBonus(uint32 bonus);

	raid();
	raid(uint32 id, string name, uint32 exp, uint32 phase, uint32 level, uint32 bonus);

private:
	uint32 id;
	string name;
    uint32 exp;
	uint32 phase;
    uint32 level;
    uint32 bonus;
};
