#include "AzthGroupMgr.h"
#include "DatabaseEnv.h"

AzthGroupMgr::AzthGroupMgr(Group* group) {
    this->group = group;
    this->levelMaxGroup = 0;
    this->groupSize = 1;
}

void AzthGroupMgr::saveToDb() {
    CharacterDatabase.PExecute("UPDATE groups SET MaxLevelGroup = %u, MaxGroupSize = %u WHERE leaderGuid = %u", this->levelMaxGroup, this->groupSize, this->group->GetLeaderGUID());
}

AzthGroupMgr::~AzthGroupMgr() {}


