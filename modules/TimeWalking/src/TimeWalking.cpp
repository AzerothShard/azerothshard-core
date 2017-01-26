#include "ScriptMgr.h"
#include "Unit.h"
#include "Pet.h"
#include "SharedDefines.h"
#include "Player.h"
#include "Common.h"
#include "raid.h"
#include "Creature.h"
#include "AzthPlayer.h"
#include "AzthLevelStat.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"

std::map<uint32, raid> raidList;
std::map<uint32, AzthLevelStat> timeWalkingLevelsStatsList;

enum npc_timewalking_enum
{
    TIMEWALKING_GOSSIP_NPC_TEXT_MAIN = 50100,
    TIMEWALKING_GOSSIP_NPC_TEXT_BONUS = 50101,
    TIMEWALKING_GOSSIP_NPC_TEXT_EXP = 50102,
    TIMEWALKING_GOSSIP_NPC_TEXT_PHASE = 50103,
    TIMEWALKING_GOSSIP_NPC_TEXT_RAID = 50104,
    TIMEWALKING_GOSSIP_NPC_TEXT_ALREADYAPPLIED = 50105,
};

class loadTimeWalkingRaid : public WorldScript
{
public:
    loadTimeWalkingRaid() : WorldScript("loadTimeWalkingRaid") {}
    void OnStartup()
    {
        QueryResult timewalking_table = ExtraDatabase.PQuery("SELECT id,name,exp,phase,level,bonus FROM timewalking ORDER BY exp, phase, level, name;"); //author, title, music id and genre from DB
        if (!timewalking_table)
        {
            sLog->outString(">> Loaded 0 raids for TimeWalking. DB table `timewalking` is empty.\n");
            sLog->outString();
            return;
        }
        Field* timeWalking_Field = timewalking_table->Fetch();

        do
        {
            raidList[timeWalking_Field[0].GetUInt32()] = raid(timeWalking_Field[0].GetUInt32(), timeWalking_Field[1].GetString(), timeWalking_Field[2].GetUInt32(), timeWalking_Field[3].GetUInt32(), timeWalking_Field[4].GetUInt32(), timeWalking_Field[5].GetUInt32());
        } while (timewalking_table->NextRow());
    
        
        
        QueryResult timewalkingLevel_table = ExtraDatabase.PQuery("SELECT level,race,class,strength_pct,agility_pct,stamina_pct,intellect_pct,spirit_pct,power_cost FROM timewalking_levels ORDER BY level;");
        if (!timewalkingLevel_table)
        {
            sLog->outString(">> Loaded 0 levels for TimeWalking. DB table `timewalking_levels` is empty.\n");
            sLog->outString();
            return;
        }

        Field* timeWalkingLevel_Field = timewalkingLevel_table->Fetch();

        do
        {
            timeWalkingLevelsStatsList[timeWalkingLevel_Field[0].GetUInt32()*10000+timeWalkingLevel_Field[1].GetUInt32()*100+timeWalkingLevel_Field[2].GetUInt32()] = AzthLevelStat(timeWalkingLevel_Field[0].GetUInt32(), timeWalkingLevel_Field[1].GetUInt32(), timeWalkingLevel_Field[2].GetUInt32(), timeWalkingLevel_Field[3].GetUInt32(), timeWalkingLevel_Field[4].GetUInt32(), timeWalkingLevel_Field[5].GetUInt32(), timeWalkingLevel_Field[6].GetUInt32(), timeWalkingLevel_Field[7].GetUInt32(), timeWalkingLevel_Field[8].GetUInt32());
        } while (timewalkingLevel_table->NextRow());

        sAzthLevelStat->SetLevelStatList(timeWalkingLevelsStatsList);
    }
};

class TimeWalkingGossip : public CreatureScript
{
public:
    TimeWalkingGossip() : CreatureScript("TimeWalkingGossip") {}

    bool OnGossipHello(Player* player, Creature* creature)
    {
        player->ADD_GOSSIP_ITEM(0, "Raid con bonus", GOSSIP_SENDER_MAIN, 4);
        player->ADD_GOSSIP_ITEM(0, "Raid standard", GOSSIP_SENDER_MAIN, 5);
        player->ADD_GOSSIP_ITEM_EXTENDED(0, "Livello specifico", GOSSIP_SENDER_MAIN, 6, "Imposta un livello", 0, true);
        player->ADD_GOSSIP_ITEM(0, "Esci dalla modalità TimeWalking", GOSSIP_SENDER_MAIN, 7);
        player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_MAIN, creature->GetGUID());
        return true;
    }

    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == 6)
        {
            int level = atoi(code);
            if (level < 0 || level > 80)
            {
                return false;
            }

            player->azthPlayer->SetTimeWalkingLevel(level);
            player->PlayerTalkClass->SendCloseGossip();
        }

    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == 4)
        {
            for (std::map<uint32, raid>::iterator it = raidList.begin(); it != raidList.end(); it++)
            {
                if (it->second.GetBonus() == 1)
                {
                    player->ADD_GOSSIP_ITEM(0, "|cFFf91616Bonus: " + it->second.GetName()+"|r", GOSSIP_SENDER_MAIN, 10000+it->second.GetLevel());   
                }
                player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_BONUS, creature->GetGUID());
            }
        }

        if (action == 5)
        {
            std::vector<uint32> expList;

            for (std::map<uint32, raid>::iterator it = raidList.begin(); it != raidList.end(); it++)
            {
                string exp;
                if (it->second.GetExp() == 1)
                {
                    exp = "Classic";
                }
                else if (it->second.GetExp() == 2)
                {
                    exp = "The Burning Crusade";
                }
                else
                {
                    exp = "Wrath of The Lich King";
                }

                if (std::find(expList.begin(), expList.end(), it->second.GetExp()) == expList.end()) {
                    expList.push_back(it->second.GetExp());
                    player->ADD_GOSSIP_ITEM(0, exp, GOSSIP_SENDER_MAIN, it->second.GetExp()); // go to phase menu
                }
            }
            player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_EXP, creature->GetGUID());
        }

        if (action <= 3) //generate phase menu
        {
            std::vector<uint32> phaseList;

            for (std::map<uint32, raid>::iterator it = raidList.begin(); it != raidList.end(); it++)
            {
                if (it->second.GetExp() == action)
                {
                    if (std::find(phaseList.begin(), phaseList.end(), it->second.GetPhase()) == phaseList.end()) {
                        phaseList.push_back(it->second.GetPhase());
                        std::stringstream s;
                        s << "Fase " << it->second.GetPhase();
                        player->ADD_GOSSIP_ITEM(0, s.str().c_str(), GOSSIP_SENDER_MAIN, 1000 + it->second.GetPhase());
                    }
                    player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_PHASE, creature->GetGUID());
                }
            }
        }
        else if(action >= 1000 && action < 10000) //generate raid menu
        {
            for (std::map<uint32, raid>::iterator it = raidList.begin(); it != raidList.end(); it++)
            {
                if (it->second.GetPhase() == action - 1000)
                {
                    player->ADD_GOSSIP_ITEM(0, it->second.GetName(), GOSSIP_SENDER_MAIN, it->second.GetLevel()+10000);
                    player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_RAID, creature->GetGUID());
                }
            }
        }
        else if (action >= 10000) //apply level
        {
            uint32 level = action - 10000;
            if (player->azthPlayer->GetTimeWalkingLevel() == NULL)
            {
                player->azthPlayer->SetTimeWalkingLevel(level);
                player->PlayerTalkClass->SendCloseGossip();
            }
            else
            {
                player->SEND_GOSSIP_MENU(TIMEWALKING_GOSSIP_NPC_TEXT_ALREADYAPPLIED, creature->GetGUID());
            }
        }
        
        else if (action == 7)
        {
            player->azthPlayer->SetTimeWalkingLevel(NULL);
            player->PlayerTalkClass->SendCloseGossip();
        }

        return true;
    }
};


class modAttackMelleeStats : public UnitScript
{
public:
    modAttackMelleeStats() : UnitScript("modAttackMelleeStats") {}
    void OnBeforeRollMeleeOutcomeAgainst(const Unit* attacker, const Unit* victim, WeaponAttackType attType, int32 &attackerMaxSkillValueForLevel, int32 &victimMaxSkillValueForLevel, int32 &attackerWeaponSkill, int32 &victimDefenseSkill, int32 &crit_chance, int32 &miss_chance, int32 &dodge_chance, int32 &parry_chance, int32 &block_chance)
    {
        if (victim->isType(TYPEMASK_PLAYER))
        {
            if (victim->ToPlayer()->azthPlayer->GetTimeWalkingLevel() != NULL)
            {
                miss_chance = (attacker->MeleeSpellMissChance(victim, attType, int32(attacker->GetWeaponSkillValue(attType, victim)) - int32(victim->ToPlayer()->azthPlayer->GetTimeWalkingLevel() * 5), 0))*100;
                attackerMaxSkillValueForLevel = attacker->GetMaxSkillValueForLevel(attacker);
                victimMaxSkillValueForLevel = victim->ToPlayer()->azthPlayer->GetTimeWalkingLevel() * 5; 
                /*attackerWeaponSkill = 0;
                victimDefenseSkill = victim->ToPlayer()->GetMaxSkillValue(SKILL_DEFENSE) + uint32(victim->ToPlayer()->GetRatingBonusValue(CR_DEFENSE_SKILL));*/
            }
        }
    }
};

class timeWalkingPlayer : public PlayerScript
{
public:
    timeWalkingPlayer() : PlayerScript("timeWalkingPlayer") {}
    void OnLogin(Player* player) 
    {
        QueryResult timewalkingCharactersActive_table = ExtraDatabase.PQuery(("SELECT id,level FROM timewalking_characters_active WHERE id = %d;"), player->GetGUID());
        //clean player to avoid problems
        player->azthPlayer->SetTimeWalkingLevel(NULL);
        if (timewalkingCharactersActive_table) //if is in timewalking mode apply debuff
        {
            Field* timewalkingCharactersActive_field = timewalkingCharactersActive_table->Fetch();

            player->azthPlayer->SetTimeWalkingLevel(timewalkingCharactersActive_field[1].GetUInt32());
        }
    }
    


    void OnBeforeInitTalentForLevel(Player* player, uint8& level, uint32& talentPointsForLevel)
    {
        if (player->azthPlayer->GetTimeWalkingLevel() != NULL)
        {
            talentPointsForLevel = 71;
        }
    }

    void OnAfterUpdateMaxPower(Player* player, Powers& power, float& value)
    {
        uint32 timeWalkingLevel = player->azthPlayer->GetTimeWalkingLevel();
        if (timeWalkingLevel != NULL)
        {
            if (power == POWER_MANA)
            {
                QueryResult getBaseMana = WorldDatabase.PQuery(("SELECT basemana FROM player_classlevelstats WHERE class = %d AND level = %d"), player->getClass(), player->azthPlayer->GetTimeWalkingLevel());
                Field* getBaseMana_field = getBaseMana->Fetch();
                uint32 baseMana = getBaseMana_field[0].GetUInt32();
                
                float intellectPct = sAzthLevelStat->GetLevelStatList()[timeWalkingLevel * 10000 + player->getRace() * 100 + player->getClass()].GetIntPct();
                float intellect = player->GetStat(STAT_INTELLECT);
                float intellectTimewalking = ((intellect * intellectPct) / 100); //to be adjusted

                float baseInt = intellectTimewalking < 20 ? intellectTimewalking : 20;
                float moreInt = intellectTimewalking - baseInt;

                uint32 manaBonus = baseInt + (moreInt*15.0f);

                value = baseMana + manaBonus;
            }
        }
    }

    void OnAfterUpdateMaxHealth(Player* player, float& value)
    {
        uint32 timeWalkingLevel = player->azthPlayer->GetTimeWalkingLevel();
        if (timeWalkingLevel != NULL)
        {
            QueryResult getBaseHp = WorldDatabase.PQuery(("SELECT basehp FROM player_classlevelstats WHERE class = %d AND level = %d"), player->getClass(), player->azthPlayer->GetTimeWalkingLevel());
            Field* getBaseHp_field = getBaseHp->Fetch();
            uint32 baseHp = getBaseHp_field[0].GetUInt32();
            
            float staminaPct = sAzthLevelStat->GetLevelStatList()[timeWalkingLevel * 10000 + player->getRace() * 100 + player->getClass()].GetStaPct();
            float stamina = player->GetStat(STAT_STAMINA);
            float staminaTimewalking = ((stamina * staminaPct) / 100); //to be adjusted

            float baseStam = staminaTimewalking < 20 ? staminaTimewalking : 20;
            float moreStam = staminaTimewalking - baseStam;

            uint32 hpBonus = baseStam + (moreStam*10.0f);
            value = baseHp + hpBonus;
        }
    }

    void OnAfterUpdateAttackPowerAndDamage(Player* player, float& level, float& base_attPower, float& attPowerMod, float& attPowerMultiplier, bool ranged)
    {
        if (player->azthPlayer->GetTimeWalkingLevel() != NULL)
        {
            float val2 = 0.0f;
            level = float(player->azthPlayer->GetTimeWalkingLevel());

            UnitMods unitMod = ranged ? UNIT_MOD_ATTACK_POWER_RANGED : UNIT_MOD_ATTACK_POWER;

            uint16 index = UNIT_FIELD_ATTACK_POWER;
            uint16 index_mod = UNIT_FIELD_ATTACK_POWER_MODS;
            uint16 index_mult = UNIT_FIELD_ATTACK_POWER_MULTIPLIER;

            float agilityPct = sAzthLevelStat->GetLevelStatList()[level * 10000 + player->getRace() * 100 + player->getClass()].GetAgiPct();
            float agility = player->GetStat(STAT_AGILITY);
            float agilityTimewalking = ((agility * agilityPct) / 100); //to be adjusted

            float strengthPct = sAzthLevelStat->GetLevelStatList()[level * 10000 + player->getRace() * 100 + player->getClass()].GetStrPct();
            float strength = player->GetStat(STAT_STRENGTH);
            float strengthTimewalking = ((strength * strengthPct) / 100); //to be adjusted

            if (ranged)
            {
                index = UNIT_FIELD_RANGED_ATTACK_POWER;
                index_mod = UNIT_FIELD_RANGED_ATTACK_POWER_MODS;
                index_mult = UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER;

                switch (player->getClass())
                {
                case CLASS_HUNTER:
                    val2 = level * 2.0f + agilityTimewalking - 10.0f;
                    break;
                case CLASS_ROGUE:
                    val2 = level + agilityTimewalking - 10.0f;
                    break;
                case CLASS_WARRIOR:
                    val2 = level + agilityTimewalking - 10.0f;
                    break;
                case CLASS_DRUID:
                    switch (player->GetShapeshiftForm())
                    {
                    case FORM_CAT:
                    case FORM_BEAR:
                    case FORM_DIREBEAR:
                        val2 = 0.0f; break;
                    default:
                        val2 = agilityTimewalking - 10.0f; break;
                    }
                    break;
                default: val2 = agilityTimewalking - 10.0f; break;
                }
            }
            else
            {
                switch (player->getClass())
                {
                case CLASS_WARRIOR:
                    val2 = level * 3.0f + strengthTimewalking  * 2.0f - 20.0f;
                    break;
                case CLASS_PALADIN:
                    val2 = level * 3.0f + strengthTimewalking  * 2.0f - 20.0f;
                    break;
                case CLASS_DEATH_KNIGHT:
                    val2 = level * 3.0f + strengthTimewalking  * 2.0f - 20.0f;
                    break;
                case CLASS_ROGUE:
                    val2 = level * 2.0f + strengthTimewalking + agilityTimewalking - 20.0f;
                    break;
                case CLASS_HUNTER:
                    val2 = level * 2.0f + strengthTimewalking + agilityTimewalking - 20.0f;
                    break;
                case CLASS_SHAMAN:
                    val2 = level * 2.0f + strengthTimewalking + agilityTimewalking - 20.0f;
                    break;
                case CLASS_DRUID:
                {
                    // Check if Predatory Strikes is skilled
                    float mLevelMult = 0.0f;
                    float weapon_bonus = 0.0f;
                    if (player->IsInFeralForm())
                    {
                        Unit::AuraEffectList const& mDummy = player->GetAuraEffectsByType(SPELL_AURA_DUMMY);
                        for (Unit::AuraEffectList::const_iterator itr = mDummy.begin(); itr != mDummy.end(); ++itr)
                        {
                            AuraEffect* aurEff = *itr;
                            if (aurEff->GetSpellInfo()->SpellIconID == 1563)
                            {
                                switch (aurEff->GetEffIndex())
                                {
                                case 0: // Predatory Strikes (effect 0)
                                    mLevelMult = CalculatePct(1.0f, aurEff->GetAmount());
                                    break;
                                case 1: // Predatory Strikes (effect 1)
                                    if (Item* mainHand = player->GetItemByPos(EQUIPMENT_SLOT_MAINHAND))
                                    {
                                        // also gains % attack power from equipped weapon
                                        ItemTemplate const* proto = mainHand->GetTemplate();
                                        if (!proto)
                                            continue;

                                        uint32 ap = proto->getFeralBonus();
                                        // Get AP Bonuses from weapon
                                        for (uint8 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
                                        {
                                            if (i >= proto->StatsCount)
                                                break;

                                            if (proto->ItemStat[i].ItemStatType == ITEM_MOD_ATTACK_POWER)
                                                ap += proto->ItemStat[i].ItemStatValue;
                                        }

                                        // Get AP Bonuses from weapon spells
                                        for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
                                        {
                                            // no spell
                                            if (!proto->Spells[i].SpellId || proto->Spells[i].SpellTrigger != ITEM_SPELLTRIGGER_ON_EQUIP)
                                                continue;

                                            // check if it is valid spell
                                            SpellInfo const* spellproto = sSpellMgr->GetSpellInfo(proto->Spells[i].SpellId);
                                            if (!spellproto)
                                                continue;

                                            for (uint8 j = 0; j < MAX_SPELL_EFFECTS; ++j)
                                                if (spellproto->Effects[j].ApplyAuraName == SPELL_AURA_MOD_ATTACK_POWER)
                                                    ap += spellproto->Effects[j].CalcValue();
                                        }

                                        weapon_bonus = CalculatePct(float(ap), aurEff->GetAmount());
                                    }
                                    break;
                                default:
                                    break;
                                }
                            }
                        }
                    }

                    switch (player->GetShapeshiftForm())
                    {
                    case FORM_CAT:
                        val2 = (player->azthPlayer->GetTimeWalkingLevel() * mLevelMult) + strengthTimewalking * 2.0f + agilityTimewalking - 20.0f + weapon_bonus /*+ m_baseFeralAP*/;
                        break;
                    case FORM_BEAR:
                    case FORM_DIREBEAR:
                        val2 = (player->azthPlayer->GetTimeWalkingLevel() * mLevelMult) + strengthTimewalking * 2.0f - 20.0f + weapon_bonus /*+ m_baseFeralAP*/;
                        break;
                    case FORM_MOONKIN:
                        val2 = (player->azthPlayer->GetTimeWalkingLevel() * mLevelMult) + strengthTimewalking * 2.0f - 20.0f /*+ m_baseFeralAP*/;
                        break;
                    default:
                        val2 = strengthTimewalking * 2.0f - 20.0f;
                        break;
                    }
                    break;
                }
                case CLASS_MAGE:
                    val2 = strengthTimewalking - 10.0f;
                    break;
                case CLASS_PRIEST:
                    val2 = strengthTimewalking - 10.0f;
                    break;
                case CLASS_WARLOCK:
                    val2 = strengthTimewalking - 10.0f;
                    break;
                }
            }

            player->SetModifierValue(unitMod, BASE_VALUE, val2);

            base_attPower = player->GetModifierValue(unitMod, BASE_VALUE) * player->GetModifierValue(unitMod, BASE_PCT);
            attPowerMod = player->GetModifierValue(unitMod, TOTAL_VALUE);

            //add dynamic flat mods
            if (ranged)
            {
                if ((player->getClassMask() & CLASSMASK_WAND_USERS) == 0)
                {
                    std::list<AuraEffect*> mRAPbyStat = player->GetAuraEffectsByType(SPELL_AURA_MOD_RANGED_ATTACK_POWER_OF_STAT_PERCENT);
                    for (std::list<AuraEffect*>::const_iterator i = mRAPbyStat.begin(); i != mRAPbyStat.end(); ++i)
                        attPowerMod += CalculatePct(player->GetStat(Stats((*i)->GetMiscValue())), (*i)->GetAmount());
                }
            }
            else
            {
                std::list<AuraEffect*> mAPbyStat = player->GetAuraEffectsByType(SPELL_AURA_MOD_ATTACK_POWER_OF_STAT_PERCENT);
                for (std::list<AuraEffect*>::const_iterator i = mAPbyStat.begin(); i != mAPbyStat.end(); ++i)
                    attPowerMod += CalculatePct(player->GetStat(Stats((*i)->GetMiscValue())), (*i)->GetAmount());

                std::list<AuraEffect*> mAPbyArmor = player->GetAuraEffectsByType(SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR);
                for (std::list<AuraEffect*>::const_iterator iter = mAPbyArmor.begin(); iter != mAPbyArmor.end(); ++iter)
                    // always: ((*i)->GetModifier()->m_miscvalue == 1 == SPELL_SCHOOL_MASK_NORMAL)
                    attPowerMod += int32(player->GetArmor() / (*iter)->GetAmount());
            }

            attPowerMultiplier = player->GetModifierValue(unitMod, TOTAL_PCT) - 1.0f;
        }
    }
};

void AddSC_TimeWalking()
{
    new loadTimeWalkingRaid();
    new TimeWalkingGossip();
    new timeWalkingPlayer();
    new modAttackMelleeStats();
}