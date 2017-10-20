#include "AzthFirstKills.h"
#include "Player.h"

#include <stdio.h>
#include <time.h>
 

void AzthFirstKills::setRealmCompleted(const AchievementEntry* achievement)
{
    this->currentFirstKills[achievement->ID] = std::chrono::system_clock::now();
} 
 
bool AzthFirstKills::isRealmCompleted(AchievementEntry const* achievement, bool originalValue) {
    if (!achievement) //should never happen
        return originalValue;
    
    // if first kill exists in our std::map and has been achieved more than 1 minute ago
    // then it has been completed.
    std::map<uint32, std::chrono::system_clock::time_point>::iterator itr=this->currentFirstKills.find(achievement->ID);
    if (itr!=this->currentFirstKills.end() 
        && (std::chrono::system_clock::now() - itr->second) > std::chrono::minutes(1))
        return true;

    time_t now = time(0);
    struct tm * tnow = std::gmtime(&now);
    
    switch(achievement->ID) {
        case ACHI_NAXXRAMAS:
            if (tnow->tm_mon >= 0) { // January : 1-1
                return false;
            }
        break;
        case ACHI_OBSIDIAN:
            if (tnow->tm_mon >= 1) {// February      
                AchievementEntry const* achievement = sAchievementStore.LookupEntry(ACHI_NAXXRAMAS);
                if (isRealmCompleted(achievement, false))
                    return false;
            }
        break;
        case ACHI_MAGIC_SEEKER:
            if (tnow->tm_mon >= 2) { // March        
                AchievementEntry const* achievement = sAchievementStore.LookupEntry(ACHI_OBSIDIAN);
                if (isRealmCompleted(achievement, false))
                    return false;
            }
        break;
        case ACHI_DEATH_DEMISE:
            if (tnow->tm_mon >= 3) { // Avril    
                AchievementEntry const* achievement = sAchievementStore.LookupEntry(ACHI_MAGIC_SEEKER);
                if (isRealmCompleted(achievement, false))
                    return false;
                return false;
            }
        break;
        case ACHI_CELESTIAL_DEFENDER:
            if (tnow->tm_mon >= 4) { // May
                AchievementEntry const* achievement = sAchievementStore.LookupEntry(ACHI_DEATH_DEMISE);
                if (isRealmCompleted(achievement, false))
                    return false;
            }
        break;
        case ACHI_GRAND_CRUSADER:
            if (tnow->tm_mon >= 7) { // August      
                AchievementEntry const* achievement = sAchievementStore.LookupEntry(ACHI_CELESTIAL_DEFENDER);
                if (isRealmCompleted(achievement, false))
                    return false;
            }
        break;
        case ACHI_FALL_OF_LK:
            if (tnow->tm_mon >= 8) { // September          
                AchievementEntry const* achievement = sAchievementStore.LookupEntry(ACHI_CELESTIAL_DEFENDER);
                if (isRealmCompleted(achievement, false))
                    return false;
            }
        break;
    }
    
    // we assume that the achievement 
    // has been completed (not available) at this point
    return true;
}


void AzthFirstKills::loadCurrentFirstkills() {

    time_t now = time(0);
    struct tm * tnow = std::gmtime(&now);
    int currentYear = tnow->tm_year + 1900;
    // is it valid until 2100?
    int epochYear = currentYear - 1970;
    int leapYears = (epochYear + 1) / 4;
    time_t result = epochYear * 24 * 60 * 60 * 365;
    result += leapYears * 24 * 60 * 60;
    
    // valid until 2106
    uint32 t = static_cast<uint32>(result);

    QueryResult fkAchievements = CharacterDatabase.PQuery("SELECT DISTINCT(achievement) FROM character_achievement WHERE achievement IN (%u,%u,%u,%u,%u,%u,%u) AND date >= %u",
        ACHI_NAXXRAMAS,
        ACHI_OBSIDIAN,
        ACHI_MAGIC_SEEKER,
        ACHI_DEATH_DEMISE,
        ACHI_CELESTIAL_DEFENDER,
        ACHI_GRAND_CRUSADER,
        ACHI_FALL_OF_LK,
        t
    );

    if (fkAchievements)
    {
        do {
            Field* fkAchievementsFields = fkAchievements->Fetch();
            
            if (!fkAchievementsFields)
                break;
            
            uint32 achievement = fkAchievementsFields[0].GetUInt32();
            // initialize all completed first kill with the timestamp of beginning of the year ( > 1 minute )
            this->currentFirstKills[achievement]=std::chrono::system_clock::from_time_t(t);
        } while (fkAchievements->NextRow());

    }
    
}