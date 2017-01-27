#ifndef AZTH_UTILS_H
#define	AZTH_UTILS_H

#include "Common.h"
#include "Define.h"
#include "Config.h"
#include "Item.h"
#include "ItemPrototype.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "SpellAuras.h"

class AuraApplication;

class AzthUtils
{
    friend class ACE_Singleton<AzthUtils, ACE_Null_Mutex>;
public:
    AzthUtils();
    ~AzthUtils();

    void removeTimewalkingAura(Unit * unit);

    float getCustomMultiplier(ItemTemplate const * pProto, uint32 multiplier);

    uint32 calculateItemScalingValue(ItemTemplate const* pProto,Player *pl = NULL);
};

#define sAzthUtils ACE_Singleton<AzthUtils, ACE_Null_Mutex>::instance()

#endif AZTH_UTILS_H