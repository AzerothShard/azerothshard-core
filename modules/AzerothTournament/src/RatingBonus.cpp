/**
    This plugin can be used for common player customizations
 */

#include "ScriptMgr.h"
#include "RatingBonus.h"
#include "Battleground.h"

class RatingBonus : public FormulaScript{
public:

    RatingBonus() : FormulaScript("RatingBonus") { }

    void OnAfterArenaRatingCalculation(Battleground *const bg, int32 &winnerMatchmakerChange, int32 &loserMatchmakerChange, int32 &winnerChange, int32 &loserChange) override
    {
        std::vector<Bonus> currBonuses = sBonusRating->getCurrentBonuses();

        // no bonuses right now
        if (currBonuses.size() == 0)
            return;

        for (int i = 0; i < currBonuses.size(); i++)
        {
            if (currBonuses[i].type == bg->GetArenaType())
            {
                winnerMatchmakerChange = winnerMatchmakerChange * currBonuses[i].multiplier;
                winnerChange = winnerChange * currBonuses[i].multiplier;
            }
        }
    }

};

void BonusRating::loadBonuses() {

    uint32 count = 0;

    QueryResult ssCommandsResult = ExtraDatabase.PQuery("SELECT id, type, day, startHour, endHour, multiplier FROM rating_bonuses;");

    if (ssCommandsResult) {
        do {
            Bonus bonus = {};
            bonus.id = (*ssCommandsResult)[0].GetUInt32();
            bonus.type = (*ssCommandsResult)[1].GetUInt32();
            bonus.weekDay = (*ssCommandsResult)[2].GetUInt32();
            bonus.startHour = (*ssCommandsResult)[3].GetUInt32();
            bonus.endHour = (*ssCommandsResult)[4].GetUInt32();
            bonus.multiplier = (*ssCommandsResult)[5].GetInt32();

            if (bonus.startHour > bonus.endHour)
                sLog->outErrorDb("RatingBonus: start time is higher than end time!");
            else
            {
                bonuses.push_back(bonus);
                count++;
            }

        } while (ssCommandsResult->NextRow());
    }

    sLog->outString("RatingBonus: loaded %u bonuses", count);
}

std::vector<Bonus> BonusRating::getRatingBonuses()
{
    return sBonusRating->bonuses;
}

std::vector<Bonus> BonusRating::getCurrentBonuses()
{
    time_t t = time(NULL);
    tm *lt = localtime(&t);

    std::vector<Bonus> bonusVect = getRatingBonuses();
    std::vector<Bonus> result = {};

    for (int i = 0; i < bonusVect.size(); i++)
    {
        Bonus bonus = bonusVect[i];
        if (bonus.weekDay == lt->tm_wday)
        {
            if (bonus.startHour <= lt->tm_hour && bonus.endHour > lt->tm_hour)
            {
                result.push_back(bonus);
            }
        }
    }


    return result;
}

class RatingBonusWorld : public WorldScript {
public:
    RatingBonusWorld() : WorldScript("RatingBonusWorld") {}

    void OnAfterConfigLoad(bool reload) override { sBonusRating->loadBonuses(); }
};

void AddAzTournamentScripts() {
    new RatingBonus();
    new RatingBonusWorld();
}

