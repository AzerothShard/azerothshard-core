#ifndef SHAREDDEFINES_H
#define SHAREDDEFINES_H

#include "Object.h"

/*
enum AzthRbac {
    RBAC_PERM_COMMAND_QUESTCOMPLETER = 1000,
    RBAC_PERM_COMMAND_AZTH = 1001,
    RBAC_PERM_COMMAND_AZTH_MAXSKILL = 1002,
    RBAC_PERM_COMMAND_AZTH_XP = 1003,
    RBAC_PERM_PVP_CUSTOM = 1004 // Arena Spectator ADDON commands
};
 */

enum SkillSpells {
    ONE_HAND_AXES = 196,
    TWO_HAND_AXES = 197,
    ONE_HAND_MACES = 198,
    TWO_HAND_MACES = 199,
    POLEARMS = 200,
    ONE_HAND_SWORDS = 201,
    TWO_HAND_SWORDS = 202,
    STAVES = 227,
    BOWS = 264,
    GUNS = 266,
    DAGGERS = 1180,
    WANDS = 5009,
    CROSSBOWS = 5011,
    FIST_WEAPONS = 15590
};

enum AzthCustomLangs {
    AZTH_LOC_IT = 0,
    AZTH_LOC_EN = 1,
};

class SharedDef {
public:
    static const WorldLocation dalaran;
};

#endif /* SHAREDDEFINES_H */

