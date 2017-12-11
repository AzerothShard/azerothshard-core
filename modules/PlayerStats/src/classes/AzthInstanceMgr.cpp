#include "AzthInstanceMgr.h"

 AzthInstanceMgr::AzthInstanceMgr(InstanceSave* is) {
     this->levelMax = 0;
     this->groupSize = 1;
     this->is = is;
 }

 void AzthInstanceMgr::saveToDb() {
     CharacterDatabase.PExecute("UPDATE instance SET levelPg = %u, groupSize = %u WHERE id = %u", this->levelMax, this->groupSize, this->is->GetInstanceId());
 }

