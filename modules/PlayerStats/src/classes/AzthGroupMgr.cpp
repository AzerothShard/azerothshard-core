#include "AzthGroupMgr.h"
#include "DatabaseEnv.h"

AzthGroupMgr::AzthGroupMgr(Group* group) {
    this->group = group;
    this->levelMaxGroup = 0;
    this->groupSize = 1;
}

void AzthGroupMgr::saveToDb() {
    CharacterDatabase.PExecute("UPDATE groups SET maxLevelGroup = %u WHERE leaderGuid = %u", this->levelMaxGroup, this->group->GetLeaderGUID());
}

AzthGroupMgr::~AzthGroupMgr() {}


