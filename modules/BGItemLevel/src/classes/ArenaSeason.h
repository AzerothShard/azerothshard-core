#include "Common.h"
#include "Define.h"
#include "Config.h"
#include "ItemPrototype.h"
#include "Player.h"

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
    
    
    bool checkItem(ItemTemplate const* proto);
    bool checkItem(ItemTemplate const* proto, Player const* player);

    Season();
    Season(int itemLevel, time_t startingDate, time_t endDate);
    std::vector<std::string> checkItems(Player *pl);
    bool canJoinArenaOrBg(Player *pl);

private:
    bool enabled;
    int itemLevel;
    time_t startingDate;
    time_t endDate;
};


#define sASeasonMgr ACE_Singleton<Season, ACE_Null_Mutex>::instance()
