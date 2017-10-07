#ifndef AZTHLANGUAGE_H
#define AZTHLANGUAGE_H

#include "Config.h"
#include "Common.h"

class Player;

enum AzthLangStrEnum {
    AZTH_LANG_REMOVED_ITEMS=0,
    AZTH_LANG_PVP_NPC_SET_ADVICE,
    AZTH_LANG_PVP_NPC_CANNOT_EQUIP,
    AZTH_LANG_RESET_AURAS_ADVICE
};

class AzthLangString {
public:
    friend class AzthLang;

    AzthLangString(std::string const def, std::string const it);
    AzthLangString() {};
    ~AzthLangString();
private:
    std::string def;
    std::string it;
};

class AzthLang {
    friend class ACE_Singleton<AzthLang, ACE_Null_Mutex>;
public:
    std::map<uint32, AzthLangString*> strings;
    explicit AzthLang();
    ~AzthLang();
    
    void loadStrings();
    const char * get(uint32 strId, Player *pl=nullptr);
    void add(uint32 strId, std::string const def, std::string const it);
};

#define sAzthLang ACE_Singleton<AzthLang, ACE_Null_Mutex>::instance()
#endif  /* AZTHLANGUAGE_H */