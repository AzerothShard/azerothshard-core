#ifndef AZTHLANGUAGE_H
#define AZTHLANGUAGE_H

#include <map>
#include "Config.h"
#include "Define.h"
#include "Player.h"

enum {
    AZTH_LANG_REMOVED_ITEMS,
};

class AzthLangString {
public:
    friend class AzthLang;

    void AzthLangString(const char* def, const char* it) {
        this->def = def;
        this->it  = it;
    }
private:
    const char *def;
    const char *it;
};

class AzthLang {    
    std::map<uint32, AzthLangString> strings;
    explicit AzthLang();
    ~AzthLang();

    const char * get(uint32 strId, Player *pl=nullptr);
    void add(uint32 strId, const char* def, const char* it);
};

#define sAzthLang ACE_Singleton<AzthLang, ACE_Null_Mutex>::instance()
#endif  /* AZTHLANGUAGE_H */