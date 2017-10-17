#include "AzthAchievement.h"


AzthAchievement::AzthAchievement()
{
    achievement = uint32(0);
    criteria = uint32(0);
    points = uint32(0); 
    category = uint32(0);
    parentCategory = uint32(0);
    difficulty = uint32(0);
    levelMax = uint32(0);
    levelMin = uint32(0);
    level = uint32(0);
    originalPoints = uint32(0);
    name = string("");
    description = string("");
    
}

AzthAchievement::AzthAchievement(uint32 achievement, uint32 criteria, uint32 points, uint32 category, uint32 parentCategory, uint32 difficulty, uint32 levelMax, uint32 levelMin, uint32 level, uint32 originalPoints, string name, string description, uint32 reward, uint32 rewardCount, uint32 killCredit)
{
    this->achievement = achievement;
    this->criteria = criteria;
    this->points = points;
    this->category = category;
    this->parentCategory = parentCategory;
    this->difficulty = difficulty;
    this->levelMax = levelMax;
    this->levelMin = levelMin;
    this->level = level;
    this->originalPoints = originalPoints;
    this->name = name;
    this->description = description;
    this->reward = reward;
    this->rewardCount = rewardCount;
    this->killCredit = killCredit;
}

uint32 AzthAchievement::GetAchievement() const
{
    return achievement;
}

uint32 AzthAchievement::GetCriteria() const
{
    return criteria;
}

uint32 AzthAchievement::GetPoints() const
{
    return points;
}

uint32 AzthAchievement::GetCategory() const
{
    return category;
}

uint32 AzthAchievement::GetParCategory() const
{
    return parentCategory;
}

uint32 AzthAchievement::GetDifficulty() const
{
    return difficulty;
}

uint32 AzthAchievement::GetLevelMax() const
{
    return levelMax;
}

uint32 AzthAchievement::GetLevelMin() const
{
    return levelMin;
}

uint32 AzthAchievement::GetLevel() const
{
    return level;
}

uint32 AzthAchievement::GetOriginalPoints() const
{
    return originalPoints;
}

string AzthAchievement::GetName() const
{
    return name;
}

string AzthAchievement::GetDescription() const
{
    return description;
}

uint32 AzthAchievement::GetReward() const
{
    return reward;
}

uint32 AzthAchievement::GetRewardCount() const
{
    return rewardCount;
}

uint32 AzthAchievement::GetKillCredit() const
{
    return killCredit;
}
