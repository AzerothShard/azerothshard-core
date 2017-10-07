#include "AzthLanguage.h"
#include "AzthPlayer.h"
#include "Player.h"

class Player;

AzthLangString::AzthLangString(std::string const def, std::string const it) {
    this->def = def;
    this->it  = it;
}

AzthLangString::~AzthLangString()
{
}


AzthLang::AzthLang()
{
}

AzthLang::~AzthLang()
{
    //for (map<uint32, AzthLangString*>::iterator it = strings.begin(); it != strings.end(); ++it)
    //    delete it->second;
}

void AzthLang::loadStrings() {
    this->add(AZTH_LANG_REMOVED_ITEMS,
              "Items removed from npc",
              "Item rimossi dall'npc");
    this->add(AZTH_LANG_PVP_NPC_SET_ADVICE,
              "Current equipped items will be removed or sent to mail, continue?",
              "Gli item attualmente equippati saranno rimossi o mandati alla mail, vuoi proseguire?");
    this->add(AZTH_LANG_PVP_NPC_CANNOT_EQUIP,
              "An item cannot be equipped, maybe it's unique and you already got",
              "Un item non può essere equippato, forse è unico e già lo possiedi");
    this->add(AZTH_LANG_RESET_AURAS_ADVICE,
              "Reset aura can only be used while in rest state and not in combat! Go to an Inn",
              "Il comando reset aura può essere utilizzato solo mentre si è in rest e non in combat. Recati in una locanda.");
}

void AzthLang::add(uint32 strId, std::string const def, std::string const it)
{
    this->strings[strId]=new AzthLangString(def,it);
}

const char * AzthLang::get(uint32 strId,Player *pl) {
    AzthCustomLangs loc = AZTH_LOC_IT;

    if (pl)
        loc = pl->azthPlayer->getCustLang();

    if ( this->strings.find(strId) != this->strings.end() ) {
        switch(loc) {
            case AZTH_LOC_IT:
                return this->strings[strId]->it.c_str();
            break;
            case AZTH_LOC_EN:
                return this->strings[strId]->def.c_str();
            break;
        }
    }

    return "Unknown Azth string";
}