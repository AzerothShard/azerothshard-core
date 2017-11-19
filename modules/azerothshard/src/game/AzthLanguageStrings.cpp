#include "AzthLanguageStrings.h"

void AzthLang::loadStrings() {
    this->add(AZTH_LANG_REMOVED_ITEMS,
              "Items removed from npc",
              "Item rimossi dall'npc");
    this->add(AZTH_LANG_PVP_NPC_SET_ADVICE,
              "Current equipped items will be moved in bag, make sure you have enough space. Do you want continue?",
              "Gli item attualmente equippati saranno spostati nella borsa, assicurati di avere abbastanza spazio. Vuoi proseguire?");
    this->add(AZTH_LANG_PVP_NPC_CANNOT_EQUIP,
              "Item |cffff0000|Hitem:%u::::::::::::|h[%s]|h|r cannot be equipped, maybe it's unique and you already got or you don't have the requirements",
              "L'item |cffff0000|Hitem:%u::::::::::::|h[%s]|h|r non può essere equippato, forse è unico e già lo possiedi oppure non hai i requisiti");
    this->add(AZTH_LANG_RESET_AURAS_ADVICE,
              "Reset aura can only be used while in rest state and not in combat! Go to an Inn",
              "Il comando reset aura può essere utilizzato solo mentre si è in rest e non in combat. Recati in una locanda.");
    this->add(AZTH_LANG_SHOW_BANK_ITEMS,
              "Show my deposited items, from %u to %u",
              "Mostra gli items depositati, da %u a %u");
}
