#include "AzthLanguage.h"

AzthLang::AzthLang()
{
    add(AZTH_LANG_REMOVED_ITEMS,"Items removed from npc","Item rimossi dall'npc");
}

const char* AzthLang::add(uint32 strId, const char* def, const char* it)
{
    strings[strId]=new AzthLangString(def,it);
}

const char * AzthLang::get(uint32 strId,Player *pl=nullptr) {
    AzthCustomLangs loc = 0;
    
    if (pl)
        loc = pl->azthPlayer->getCustomLang();
    
    if ( strings.find(strId) == strings.end() ) {
        switch(loc) {
            case AZTH_LOC_IT:
                return strings[strId]->it;
            break;
            case AZTH_LOC_EN:
                return strings[strId]->def;
            break;
        }
    }
    
    return "Unknown Azth string";
}