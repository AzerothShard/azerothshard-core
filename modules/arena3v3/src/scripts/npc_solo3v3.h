#ifndef SOLO_3V3_H
#define SOLO_3V3_H

#include "Player.h"

class Player;

// TalentTab.dbc -> TalentTabID
const uint32 FORBIDDEN_TALENTS_IN_1V1_ARENA[] =
{
    // Healer
    201, // PriestDiscipline
    202, // PriestHoly
    382, // PaladinHoly
    262, // ShamanRestoration
    282, // DruidRestoration

         // Tanks
         //383, // PaladinProtection
         //163, // WarriorProtection

         0 // End
};


// Return false, if player have invested more than 35 talentpoints in a forbidden talenttree.
bool Arena1v1CheckTalents(Player* player);


// SOLO_3V3_TALENTS found in: TalentTab.dbc -> TalentTabID

// Warrior, Rogue, Deathknight etc.
const uint32 SOLO_3V3_TALENTS_MELEE[] =
{
    383, // PaladinProtection
    163, // WarriorProtection
    161,
    182,
    398,
    164,
    181,
    263,
    281,
    399,
    183,
    381,
    400,
    0 // End
};

// Mage, Hunter, Warlock etc.
const uint32 SOLO_3V3_TALENTS_RANGE[] =
{
    81,
    261,
    283,
    302,
    361,
    41,
    303,
    363,
    61,
    203,
    301,
    362,
    0 // End
};

const uint32 SOLO_3V3_TALENTS_HEAL[] =
{
    201, // PriestDiscipline
    202, // PriestHoly
    382, // PaladinHoly
    262, // ShamanRestoration
    282, // DruidRestoration
    0 // End
};

enum Solo3v3TalentCat
{
    MELEE = 0, 
    RANGE,
    HEALER,
    MAX_TALENT_CAT
};

// Returns MELEE, RANGE or HEALER (depends on talent builds)
Solo3v3TalentCat GetTalentCatForSolo3v3(Player* player);


#endif