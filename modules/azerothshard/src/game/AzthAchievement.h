#ifndef AZTH_ACHIEVEMENT_H
#define	AZTH_ACHIEVEMENT_H

#include "Common.h"
#include "Define.h"
#include "Config.h"

class AzthAchievement
{
    friend class ACE_Singleton<AzthAchievement, ACE_Null_Mutex>;
public:
    //GETTERS 
    uint32 GetAchievement() const;
    uint32 GetCriteria() const;
    uint32 GetPoints() const;
    uint32 GetCategory() const;
    uint32 GetParCategory() const;
    uint32 GetDifficulty() const;
    uint32 GetLevelMax() const;
    uint32 GetLevelMin() const;
    uint32 GetLevel() const;
    uint32 GetOriginalPoints() const;
    string GetName() const;
    string GetDescription() const;

    map<uint32, AzthAchievement> GetAchievementList() const;

    void SetAchievementList(map<uint32, AzthAchievement> achievementList);

    AzthAchievement();
    AzthAchievement(uint32 achievement, uint32 criteria, uint32 points, uint32 category, uint32 parentCategory, uint32 difficulty, uint32 levelMax, uint32 levelMin, uint32 level, uint32 originalPoints, string name, string description);


private:
    uint32 achievement;
    uint32 criteria;
    uint32 points;
    uint32 category;
    uint32 parentCategory;
    uint32 difficulty;
    uint32 levelMax;
    uint32 levelMin;
    uint32 level;
    uint32 originalPoints;
    string name;
    string description;
    map<uint32, AzthAchievement> achievementList;
};

#define sAzthAchievement ACE_Singleton<AzthAchievement, ACE_Null_Mutex>::instance()

#endif AZTH_ACHIEVEMENT_H