#ifndef SMARTSTONE_H
#define SMARTSTONE_H

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Player.h"
#include "WorldSession.h"
#include "WorldPacket.h"
#include "Chat.h"
#include "Spell.h"
#include "Define.h"
#include "GossipDef.h"
#include "Item.h"
#include "Common.h"
#include "Opcodes.h"
#include "Log.h"
#include "ObjectMgr.h"

struct SmartStoneCommands
{
    uint32 id;
    std::string text;
    uint32 item;
};

class SmartStone
{
public:

    int SMARTSTONE_VENDOR_ENTRY = 170000;

    std::map<uint32, std::string> ssCommands;

    std::map<uint32, uint32> ssCommandsItemRelation;

    void loadCommands();
    void SmartStoneSendListInventory(WorldSession * session, uint32 extendedCostStartValue);
};

#define sSmartStone ACE_Singleton<SmartStone, ACE_Null_Mutex>::instance()

#endif