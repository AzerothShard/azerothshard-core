#include "Common.h"
#include "Define.h"
#include "Config.h"

class Season
{
    friend class ACE_Singleton<Season, ACE_Null_Mutex>;
public:
    //GETTERS
    int GetItemLevel() const;
    time_t GetStartingDate() const;
    time_t GetEndDate() const;
    bool IsEnabled() const;

    //SETTERS
    void SetItemLevel(int itemLevel);
    void SetStartingDate(time_t startingDate);
    void SetEndDate(time_t endDate);
    void SetEnabled(bool enable);

    Season();
    Season(int itemLevel, time_t startingDate, time_t endDate);

private:
    bool enabled;
    int itemLevel;
    time_t startingDate;
    time_t endDate;
};


#define sASeasonMgr ACE_Singleton<Season, ACE_Null_Mutex>::instance()
