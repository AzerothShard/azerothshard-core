#include "ScriptMgr.h"
#include "Player.h"
#include "Item.h"
#include "ConditionMgr.h"
#include "ScriptedGossip.h"
#include "GossipDef.h"
#include "AzthLanguage.h"
#include "AzthUtils.h"

class PhaseDimensionsNPC : public CreatureScript
{
public:
    PhaseDimensionsNPC() : CreatureScript("PhaseDimensionsNPC") {}

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        //                              icon            text                           sender      action   popup message    money  code
        if (!(player->azthPlayer->GetTimeWalkingLevel() > 0) && !player->azthPlayer->hasGear())
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Deposita item", GOSSIP_SENDER_MAIN, 501);
        }
        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "Inserisci gli item da cercare qui", GOSSIP_SENDER_MAIN, 500, "Inserisci nome item", 0, true);

        player->SEND_GOSSIP_MENU(1, creature->GetGUID());
        return true;
    }


    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
      
        

        if (action == 501 && sender == GOSSIP_SENDER_MAIN)
        {
            
        }
        
        // -------->

        return true;
    }
};

class PhaseDimensions : public PlayerScript
{
public:
    PhaseDimensions() : PlayerScript("PhaseDimensions") {}

    
};

void AddSC_PhaseDimensions()
{
    new PhaseDimensions();
    new PhaseDimensionsNPC();
}