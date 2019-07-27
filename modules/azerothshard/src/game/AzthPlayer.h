#ifndef AZTHPLAYER_H
#define AZTHPLAYER_H

#include "AzthSmartStone.h"
#include "Config.h"
#include "Define.h"
#include "CustomRates.h"
#include "AzthSharedDefines.h"
#include <unordered_map>

struct SmartStoneCommand;

class CrossFaction;
class Player;
class SmartStone;
class WorldLocation;

enum AzthQueueMasks
{
    AZTH_QUEUE_BG           = 1,
    AZTH_QUEUE_ARENA        = 2,
    AZTH_QUEUE_BG_OR_ARENA  = 3, // mask BG+Arena
    AZTH_QUEUE_LFG          = 4
};

//[AZTH] PVP Rank Patch
enum HonorKillPvPRank
{
    HKRANK00,
    HKRANK01,
    HKRANK02,
    HKRANK03,
    HKRANK04,
    HKRANK05,
    HKRANK06,
    HKRANK07,
    HKRANK08,
    HKRANK09,
    HKRANK10,
    HKRANK11,
    HKRANK12,
    HKRANK13,
    HKRANK14,
    HKRANKMAX
};

struct WowarmoryFeedEntry
{
    uint32 guid;         // Player GUID
    time_t date;         // Log date
    uint32 type;         // TYPE_ACHIEVEMENT_FEED, TYPE_ITEM_FEED, TYPE_BOSS_FEED
    uint32 data;         // TYPE_ITEM_FEED: item_entry, TYPE_BOSS_FEED: creature_entry
    uint32 item_guid;    // Can be 0
    uint32 item_quality; // Can be 0
    uint8  difficulty;   // Can be 0
    int    counter;      // Can be 0
};

typedef std::vector<WowarmoryFeedEntry> WowarmoryFeeds;

//[AZTH] Custom Titles
#define PLAYER_TITLE_MASK_ALLIANCE_PVP                                      \
    (PLAYER_TITLE_PRIVATE | PLAYER_TITLE_CORPORAL |                         \
      PLAYER_TITLE_SERGEANT_A | PLAYER_TITLE_MASTER_SERGEANT |              \
      PLAYER_TITLE_SERGEANT_MAJOR | PLAYER_TITLE_KNIGHT |                   \
      PLAYER_TITLE_KNIGHT_LIEUTENANT | PLAYER_TITLE_KNIGHT_CAPTAIN |        \
      PLAYER_TITLE_KNIGHT_CHAMPION | PLAYER_TITLE_LIEUTENANT_COMMANDER |    \
      PLAYER_TITLE_COMMANDER | PLAYER_TITLE_MARSHAL |                       \
      PLAYER_TITLE_FIELD_MARSHAL | PLAYER_TITLE_GRAND_MARSHAL)

#define PLAYER_TITLE_MASK_HORDE_PVP                             \
    (PLAYER_TITLE_SCOUT | PLAYER_TITLE_GRUNT |                  \
      PLAYER_TITLE_SERGEANT_H | PLAYER_TITLE_SENIOR_SERGEANT |  \
      PLAYER_TITLE_FIRST_SERGEANT | PLAYER_TITLE_STONE_GUARD |  \
      PLAYER_TITLE_BLOOD_GUARD | PLAYER_TITLE_LEGIONNAIRE |     \
      PLAYER_TITLE_CENTURION | PLAYER_TITLE_CHAMPION |          \
      PLAYER_TITLE_LIEUTENANT_GENERAL | PLAYER_TITLE_GENERAL |  \
      PLAYER_TITLE_WARLORD | PLAYER_TITLE_HIGH_WARLORD)

#define PLAYER_TITLE_MASK_ALL_PVP  \
    (PLAYER_TITLE_MASK_ALLIANCE_PVP | PLAYER_TITLE_MASK_HORDE_PVP)

class AzthPlayer
{
public:
    explicit AzthPlayer(Player* origin);
    ~AzthPlayer();

    struct AzthAchiData
    {
        uint32 level;
        uint32 levelParty;
        uint32 specialLevel;
    };

    typedef std::unordered_map<uint16 /*achiId*/, AzthAchiData /*data*/> CompletedAchievementMap;
    typedef std::unordered_map<uint16 /*critId*/, AzthAchiData /*data*/> CompletedCriteriaMap;
    typedef std::unordered_map<uint32 /*entry*/, uint32 /*guid*/> ItemInBankMap;
    std::vector<SmartStonePlayerCommand>& getSmartStoneCommands();
    std::map<uint32, WorldLocation> getLastPositionInfoFromDB();

    // WInfi: TODO private
    CompletedAchievementMap m_completed_achievement_map;
    CompletedCriteriaMap m_completed_criteria_map;

    void SetTempGear(bool hasGear);
    bool hasGear();
    void SetPlayerQuestRate(float rate);

    /*
        Get player quest rate. If player hasn't set it before, returns server
        configuration

        @returns Quest Rate. If player set his own, return it; if he hasn't it
        returns server default
     */
    float GetPlayerQuestRate();

    uint32 getArena1v1Info(uint8 type);
    void setArena1v1Info(uint8 type, uint32 value);
    uint32 getArena3v3Info(uint8 type);
    void setArena3v3Info(uint8 type, uint32 value);    

    void ForceKilledMonsterCredit(uint32 entry, uint64 guid);
    bool passHsChecks(Quest const* qInfo, uint32 entry, uint32& realEntry, uint64 guid);
    time_t lastSent = time(nullptr);
    
    void addSmartStoneCommand(SmartStonePlayerCommand command, bool query);
    void addSmartStoneCommand(uint32 id, bool query, uint64 dateExpired, int32 charges);
    void removeSmartStoneCommand(SmartStonePlayerCommand command, bool query);
    void decreaseSmartStoneCommandCharges(uint32 id);
    bool BuySmartStoneCommand(uint64 vendorguid, uint32 vendorslot, uint32 item, uint8 count, uint8 bag, uint8 slot);
    bool hasSmartStoneCommand(uint32 id);
    void loadAzthPlayerFromDB();
    bool AzthMaxPlayerSkill();
    bool AzthSelfChangeXp(float rate);
    WorldLocation& getLastPositionInfo(uint32 dimension);
    void setLastPositionInfo(uint32 dimension, WorldLocation posIinfo);
    bool isInBlackMarket();
    void saveLastPositionInfoToDB(Player* pl);
    bool isPvP();
    void loadPvPInfo();
    AzthCustomLangs getCustLang();
    void setCustLang(AzthCustomLangs code);
    bool isPvPFlagOn(bool def);
    bool isFFAPvPFlagOn(bool def);
    void AddBankItem(uint32 itemEntry, uint32 itemGuid);
    void DelBankItem(uint32 itemEntry);
    void SetBankItemsList(ItemInBankMap itemsInBankList);
    ItemInBankMap& GetBankItemsList();
    bool canEnterMap(MapEntry const* entry, InstanceTemplate const* instance = nullptr, bool loginCheck = false);
    bool canJoinQueue(AzthQueueMasks type);
    bool canGroup(Player* with = nullptr);
    bool canExplore();
    bool canCompleteCriteria(AchievementCriteriaEntry const* criteria);
    bool canEquipItem(ItemTemplate const* proto);
    bool checkItems(uint32 iLvlMax, uint8 type = 0);
    bool checkItem(ItemTemplate const* proto);
    uint32 getMaxItemLevelByStatus();

    //TIMEWALKING
    bool canUseItem(Item* item, bool notify = false);
    bool itemCheckReqLevel(ItemTemplate const* item, bool notify = false);
    uint32 getPStatsLevel(bool normalized = true, bool checkInstance = true, bool checkGroup = true);
    uint32 getGroupLevel(bool normalized = true, bool checkInstance = true);
    uint32 getInstanceLevel(bool normalized = true);
    uint32 normalizeLvl(uint32 level);
    uint32 getGroupSize(bool checkInstance = true);
    uint32 getInstanceSize();
    uint32 GetTimeWalkingLevel() const;
    bool isTimeWalking(bool skipSpecial = false) const;
    void SetTimeWalkingLevel(uint32 timeWalkingLevel, bool clearAuras = true, bool save = true, bool login = false);
    void prepareTwSpells(uint32 oldLevel);
    uint32 getTwItemLevel(uint32 twLevel);
    time_t autoScalingPending;

    //DIMENSIONS
    uint32 getCurrentDimensionByMark() const;
    uint32 getCurrentDimensionByPhase() const;
    uint32 getCurrentDimensionByAura() const;
    bool hasDimSpecificAura(uint32 dim);
    bool changeDimension(uint32 dim, bool validate = false, bool temp = false);

    // WInfi: TODO private
    uint32 instanceID;

    // World of Warcraft Armory Feeds
    void InitWowarmoryFeeds();
    void CreateWowarmoryFeed(uint32 type, uint32 data, uint32 item_guid, uint32 item_quality);

    // World of Warcraft Armory Feeds
    WowarmoryFeeds m_wowarmory_feeds;

private:
    Player* player;
    uint32 arena1v1Info[7]; // ARENA_TEAM_END
    uint32 arena3v3Info[7]; // ARENA_TEAM_END
    uint32 levelPlayer;
    uint32 tmpLevelPg;
    uint32 groupLevel;
    
    float playerQuestRate, maxQuestRate;    
    bool m_isPvP;
    AzthCustomLangs customLang;
    uint32 timeWalkingLevel = 0;
    bool PVPTempGear = false;
    
    ItemInBankMap m_itemsInBank;
    std::vector<SmartStonePlayerCommand> smartStoneCommands;
    std::map<uint32, WorldLocation> lastPositionInfo;
};

#endif /* AZTHPLAYER_H */
