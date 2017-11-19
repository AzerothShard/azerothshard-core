#ifndef AZTHLANGUAGE_H
#define AZTHLANGUAGE_H

#include "Config.h"
#include "Common.h"

class Player;

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
    UNORDERED_MAP<uint32, AzthLangString*> strings;
    explicit AzthLang();
    ~AzthLang();
    
    void loadStrings();
    virtual const char * get(uint32 strId, Player *pl=nullptr) const;
    virtual const char * getf(uint32 strId, Player *pl=nullptr, ...) const;
    void add(uint32 strId, std::string const def, std::string const it);
};

#define sAzthLang ACE_Singleton<AzthLang, ACE_Null_Mutex>::instance()
#endif  /* AZTHLANGUAGE_H */
