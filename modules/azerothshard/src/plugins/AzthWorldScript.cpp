#include "Configuration/Config.h"
#include "AzthLanguage.h"
#include "AzthUtils.h"
#include "ScriptMgr.h"
#include "AzthFirstKills.h"

class AzthWorldScript : public WorldScript
{
public:
    AzthWorldScript() : WorldScript("AzthWorldScript") { }

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload) {
            std::string conf_path = _CONF_DIR;
            std::string cfg_file = conf_path + "/azth_mod.conf";
            std::string cfg_def_file = cfg_file;
            cfg_def_file += ".dist";

            sConfigMgr->LoadMore(cfg_def_file.c_str());
            
            sConfigMgr->LoadMore(cfg_file.c_str());
        }
    }
    
    void OnStartup() override
    {
        sAzthLang->loadStrings();
        sAzthUtils->loadClassSpells();
        sAzthFirstKills->loadCurrentFirstkills();
    }
};

void AddSC_AzthWorldScript()
{
    new AzthWorldScript();
}
