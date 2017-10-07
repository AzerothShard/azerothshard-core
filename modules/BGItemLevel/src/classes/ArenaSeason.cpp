#include "ArenaSeason.h"

Season::Season()
{
    itemLevel = uint32(0);
    startingDate = time_t(0);
    endDate = time_t(0);
}

Season::Season(int ItsItemLevel, time_t ItsStartingDate, time_t ItsEndDate)
{
    itemLevel = ItsItemLevel;
    startingDate = ItsStartingDate;
    endDate = ItsEndDate;
}

int Season::GetItemLevel() const
{
    return itemLevel;
}


void Season::SetItemLevel(int ItsItemLevel)
{
    itemLevel = ItsItemLevel;
}

time_t Season::GetStartingDate() const
{
    return startingDate;
}


void Season::SetStartingDate(time_t ItsStartingDate)
{
    startingDate = ItsStartingDate;
}

time_t Season::GetEndDate() const
{
    return endDate;
}

void Season::SetEndDate(time_t ItsEndDate)
{
    endDate = ItsEndDate;
}

bool Season::IsEnabled() const
{
    return enabled;
}

void Season::SetEnabled(bool enable)
{
    this->enabled = enable;
}

bool Season::checkItem(ItemTemplate const* proto, Player const* player) {
    if (player->InBattleground() || player->InArena() || player->InBattlegroundQueue()) {
        return this->checkItem(proto);
    }
    
    return true;
}

bool Season::checkItem(ItemTemplate const* proto) {
    // return false if item level > current season
    return !(proto->InventoryType > 0 && proto->InventoryType != INVTYPE_AMMO && proto->ItemLevel > GetItemLevel());
}

std::vector<std::string> Season::checkItems(Player *pl) {
    std::vector<std::string> incompatibleItems;
    
    if (!IsEnabled())
    {
        return incompatibleItems; //SYSTEM DISABLED
    }

    if (pl->IsGameMaster())
    {
        return incompatibleItems;
    }

    uint32 INVENTORY_END = 18;
    uint32 counter = 0;

    bool incompatible = false;

    for (uint32 INVENTORY_INDEX = 0; INVENTORY_INDEX <= INVENTORY_END; INVENTORY_INDEX++)
    {
        Item* itemToCheck = pl->GetItemByPos(INVENTORY_SLOT_BAG_0, INVENTORY_INDEX);
        if (itemToCheck != nullptr)
        {
            if (!sASeasonMgr->checkItem(itemToCheck->GetTemplate()))
            {
              ChatHandler(pl->GetSession()).PSendSysMessage("|cffff0000%s|r ha un livello troppo alto! Rimuovilo per poter giocare questa season.", itemToCheck->GetTemplate()->Name1.c_str());
              incompatibleItems.push_back(itemToCheck->GetTemplate()->Name1);
              counter++;
            }
        }
    }  
    
    ChatHandler(pl->GetSession()).PSendSysMessage("L'attuale Season ha livello massimo |cffff0000%d|r", sASeasonMgr->GetItemLevel());
    
    return incompatibleItems;
}

bool Season::canJoinArenaOrBg(Player *pl) {
    if (sASeasonMgr->checkItems(pl).empty()) {
        return true;
    }
    
    return false;
}
