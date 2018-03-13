#include "AzthInstanceMgr.h"

 AzthInstanceMgr::AzthInstanceMgr(InstanceSave* is) {
     this->levelMax = 0;
     this->groupSize = 1;
     this->startTime = static_cast<uint32>(time(NULL));
     this->is = is;
 }

 void AzthInstanceMgr::saveToDb() {
     CharacterDatabase.PExecute("UPDATE instance SET levelPg = %u, groupSize = %u, startTime = %u WHERE id = %u", this->levelMax, this->groupSize, this->is->GetInstanceId(), this->startTime);
 }

