#ifndef ARENABONUS_H
#define ARENABONUS_H

#include "Define.h"
#include "Common.h"

struct Bonus
{
    uint32 id;
    uint32 type;
    uint32 weekDay;
    uint32 startHour;
    uint32 endHour;
    int32 multiplier;
};

class BonusRating
{
    public:
        void loadBonuses();
        std::vector<Bonus> getRatingBonuses();
        std::vector<Bonus> getCurrentBonuses();

    private:
        std::vector<Bonus> bonuses;
};

#define sBonusRating ACE_Singleton<BonusRating, ACE_Null_Mutex>::instance()

#endif