#include "AzthFirstKills.h"
#include "Player.h"

#include <stdio.h>
#include <time.h>
 

bool AzthFirstKills::isRealmCompleted(AchievementEntry const* achievement, bool originalValue) {
    if (!achievement) //should never happen
        return originalValue;
    
    if (this->currentFirstKills.find(achievement->ID) != this->currentFirstKills.end())
        return true;
    
    time_t now = time(0);
    struct tm * tnow = std::gmtime(&now);
    
    switch(achievement->ID) {
        /*case ACHI_NAXXRAMAS:

            this->currentFirstKills.insert(ACHI_NAXXRAMAS);
            
            return false;
        break;
        case ACHI_OBSIDIAN:
            this->currentFirstKills.insert(ACHI_OBSIDIAN);
            
            return false;
        break;
        case ACHI_OBSIDIAN:
            this->currentFirstKills.insert(ACHI_OBSIDIAN);
            
            return false;
        break;
        case ACHI_MAGIC_SEEKER:
            this->currentFirstKills.insert(ACHI_OBSIDIAN);
            
            return false;
        break;*/
        /*case ACHI_DEATH_DEMISE:
            this->currentFirstKills.insert(ACHI_OBSIDIAN);
            
            return false;
        break;
        case ACHI_CELESTIAL_DEFENDER:
            this->currentFirstKills.insert(ACHI_OBSIDIAN);
            
            return false;
        break;
        case ACHI_GRAND_CRUSADER:
            this->currentFirstKills.insert(ACHI_OBSIDIAN);
            
            return false;
        break;*/
        case ACHI_FALL_OF_LK:
            if (tnow->tm_mon >= 9) // October : 10-1           
                return false;
        break;
    }
    
    return originalValue; // shouldn't happen
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
            this->currentFirstKills.insert(achievement);
        } while (fkAchievements->NextRow());

    }
    
}