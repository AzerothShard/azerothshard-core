#ifndef SMARTSTONE_H
#define SMARTSTONE_H

#include "Define.h"
#include "Common.h"

class WorldSession;

struct SmartStoneCommand
{
    // struct doesn't have equal operator by default
    bool operator==(const SmartStoneCommand& rhs)
    {
        return id == rhs.id;
    }

    uint32 id;
    std::string text;
    uint32 item;
    uint32 icon;
    uint32 parent_menu;
    uint32 type;
    uint32 action;
};

enum MenuType {
    DO_SCRIPTED_ACTION = 1,
    OPEN_CHILD = 2
    // DO_DB_ACTION=3 [TO IMPLEMENT]
};

class SmartStone
{
public:

    int SMARTSTONE_VENDOR_ENTRY = 170000;

    //std::map<uint32, std::string> ssCommands;

    // need improvements
    //std::map<uint32, uint32> ssCommandsIconRelation;

    //std::map<uint32, uint32> ssCommandsItemRelation;

    const SmartStoneCommand nullCommand = SmartStoneCommand{ NULL, "", NULL, NULL, NULL, NULL, NULL };

    std::vector<SmartStoneCommand> ssCommands2;

    void loadCommands();
    void SmartStoneSendListInventory(WorldSession * session, uint32 extendedCostStartValue);
    SmartStoneCommand getCommandById(uint32 id);
    SmartStoneCommand getCommandByItem(uint32 item);
    bool isNullCommand(SmartStoneCommand command);
};

#define sSmartStone ACE_Singleton<SmartStone, ACE_Null_Mutex>::instance()

#endif