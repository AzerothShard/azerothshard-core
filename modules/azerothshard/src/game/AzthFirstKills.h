#ifndef AZTH_FIRST_KILL_H
#define AZTH_FIRST_KILL_H

#include "Common.h"
#include <ace/Singleton.h>
#include "DatabaseEnv.h"
#include "DBCEnums.h"
#include "DBCStores.h"

class AzthFirstKills {
    friend class ACE_Singleton<AzthFirstKills, ACE_Null_Mutex>;
public:
     bool isRealmCompleted(AchievementEntry const* achievement, bool originalValue);   
};

#define sAzthFirstKills ACE_Singleton<AzthFirstKills, ACE_Null_Mutex>::instance()

#endif