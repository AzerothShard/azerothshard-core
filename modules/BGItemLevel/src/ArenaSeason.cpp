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
