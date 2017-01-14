#include "Common.h"

class Season
{
    friend class ACE_Singleton<Season, ACE_Null_Mutex>;
public:
    //GETTERS
    int GetItemLevel() const;
    time_t GetStartingDate() const;
    time_t GetEndDate() const;

    //SETTERS
    void SetItemLevel(int itemLevel);
    void SetStartingDate(time_t startingDate);
    void SetEndDate(time_t endDate);

    Season();
    Season(int itemLevel, time_t startingDate, time_t endDate);

private:
    int itemLevel;
    time_t startingDate;
    time_t endDate;
};


#define sASeasonMgr ACE_Singleton<Season, ACE_Null_Mutex>::instance()
