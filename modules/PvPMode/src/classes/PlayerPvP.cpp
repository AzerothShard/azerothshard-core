#include "AzthPlayer.h"
#include "Define.h"
#include "ObjectAccessor.h"
#include "World.h"
#include "Player.h"


void AzthPlayer::loadPvPInfo() {
    m_isPvP = false;
    
    uint32 accId = this->player->GetSession()->GetAccountId();
    QueryResult isPvPAccount = ExtraDatabase.PQuery("SELECT isPvP FROM pvp_accounts WHERE id = '%d';", accId);

    if (isPvPAccount != nullptr && isPvPAccount->GetRowCount() > 0)
        this->m_isPvP = true;

    if (!this->m_isPvP) { 
        uint32 characterId = this->player->GetGUID();
        QueryResult isPvPCharacter = ExtraDatabase.PQuery("SELECT isPvP FROM pvp_characters WHERE id = '%d';", characterId);

        if (isPvPCharacter != nullptr && isPvPCharacter->GetRowCount() > 0)
            this->m_isPvP = true;
    }
}

bool AzthPlayer::isPvP() {
    return this->m_isPvP;
}