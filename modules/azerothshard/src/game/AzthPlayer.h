#ifndef AZTHPLAYER_H
#define AZTHPLAYER_H

#include "AzthSmartStone.h"
#include "Config.h"
#include "Define.h"
#include <UnorderedMap.h>
#include "CustomRates.h"
#include "AzthSharedDefines.h"

struct SmartStoneCommand;

class CrossFaction;
class Player;
class SmartStone;
class WorldLocation;

enum AzthQueueMasks {
    AZTH_QUEUE_BG=1,
    AZTH_QUEUE_ARENA=2,
    AZTH_QUEUE_BG_OR_ARENA=3, // mask BG+Arena
    AZTH_QUEUE_LFG=4
};

class AzthPlayer {
public:
    void SetTempGear(bool hasGear);
    bool hasGear();
    explicit AzthPlayer(Player *origin);
    ~AzthPlayer();

  void SetPlayerQuestRate(float rate);

  /**
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

  uint32 levelPlayer;
  uint32 tmpLevelPg;
  uint32 groupLevel;

  struct AzthAchiData {
    uint32 level;
    uint32 levelParty;
  };

  typedef UNORDERED_MAP<uint16 /*achiId*/, AzthAchiData /*data*/>
      CompletedAchievementMap;
  CompletedAchievementMap m_completed_achievement_map;

  typedef UNORDERED_MAP<uint16 /*critId*/, AzthAchiData /*data*/>
      CompletedCriteriaMap;
  CompletedCriteriaMap m_completed_criteria_map;
  
  typedef UNORDERED_MAP<uint32 /*entry*/, uint32 /*guid*/> ItemInBankMap;

  uint32 instanceID;

  void ForceKilledMonsterCredit(uint32 entry, uint64 guid);
  bool passHsChecks(Quest const* qInfo, uint32 entry, uint32 &realEntry, uint64 guid);
  time_t lastSent = time(NULL);

  std::vector<SmartStonePlayerCommand> & getSmartStoneCommands();
  void addSmartStoneCommand(SmartStonePlayerCommand command, bool query);
  void addSmartStoneCommand(uint32 id, bool query, uint64 dateExpired,
                            int32 charges);
  void removeSmartStoneCommand(SmartStonePlayerCommand command, bool query);
  void decreaseSmartStoneCommandCharges(uint32 id);
  bool BuySmartStoneCommand(uint64 vendorguid, uint32 vendorslot, uint32 item,
                            uint8 count, uint8 bag, uint8 slot);
  bool hasSmartStoneCommand(uint32 id);

  void loadAzthPlayerFromDB();
  bool AzthMaxPlayerSkill();
  bool AzthSelfChangeXp(float rate);
  WorldLocation& getLastPositionInfo(uint32 dimension);
  void setLastPositionInfo(uint32 dimension, WorldLocation posIinfo);
  bool isInBlackMarket();
  std::map<uint32,WorldLocation> getLastPositionInfoFromDB();
  void saveLastPositionInfoToDB(Player *pl);

  bool isPvP();
  void loadPvPInfo();
  AzthCustomLangs getCustLang();
  void setCustLang(AzthCustomLangs code);
  
  bool isPvPFlagOn(bool def);
  bool isFFAPvPFlagOn(bool def);
  
  void AddBankItem(uint32 itemEntry, uint32 itemGuid);
  void DelBankItem(uint32 itemEntry);
  void SetBankItemsList(ItemInBankMap itemsInBankList);
  ItemInBankMap & GetBankItemsList();
  bool canEnterMap(MapEntry const* entry, InstanceTemplate const* instance = nullptr, bool loginCheck = false);
  bool canJoinQueue(AzthQueueMasks type);
  bool canGroup(Player *with=nullptr);
  bool canExplore();
  bool canCompleteCriteria(AchievementCriteriaEntry const* criteria);
  bool canEquipItem(ItemTemplate const* proto);
  bool checkItems(uint32 iLvlMax, uint8 type = 0);
  bool checkItem(ItemTemplate const* proto);
  uint32 getMaxItemLevelByStatus();

  //TIMEWALKING
  uint32 getGroupLevel(bool normalized = true);
  uint32 GetTimeWalkingLevel() const;
  bool isTimeWalking(bool skipSpecial=false) const;
  void SetTimeWalkingLevel(uint32 timeWalkingLevel, bool clearAuras = true, bool save = true, bool login = false);
  uint32 getTwItemLevel(uint32 twLevel);
  time_t autoScalingPending;
  
  //DIMENSIONS
  uint32 getCurrentDimensionByMark() const;
  uint32 getCurrentDimensionByPhase() const;
  uint32 getCurrentDimensionByAura() const;
  bool hasDimSpecificAura(uint32 dim);
  bool changeDimension(uint32 dim, bool validate = false, bool temp = false);

private:
  Player *player;
  uint32 arena1v1Info[7]; // ARENA_TEAM_END
  uint32 arena3v3Info[7]; // ARENA_TEAM_END
  float playerQuestRate, maxQuestRate;
  std::vector<SmartStonePlayerCommand> smartStoneCommands;
  std::map<uint32,WorldLocation> lastPositionInfo;
  bool m_isPvP;
  AzthCustomLangs customLang;
  
  uint32 timeWalkingLevel = 0;
  bool PVPTempGear = false;

  ItemInBankMap  m_itemsInBank;
};

#endif /* AZTHPLAYER_H */
