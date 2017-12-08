#include "AzthSmartStone.h"
#include "Chat.h"
#include "Common.h"
#include "Define.h"
#include "GossipDef.h"
#include "Item.h"
#include "Group.h"
#include "ItemPrototype.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Spell.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "MapManager.h"
#include "Map.h"
#include "Pet.h"
#include "Vehicle.h"
#include "AzthLanguageStrings.h"

class Pet;
class Group;

enum AzthSummonType {
    // special
    AZTH_SUMMON_MORPH,
    AZTH_SUMMON_MOUNT,
    AZTH_SUMMON_MOUNT_FLY,
    // pet
    AZTH_SUMMON_GUARDIAN,
    AZTH_SUMMON_POKEMON_GUARDIAN,
    // temp summons
    AZTH_SUMMON_START_MINIONS,
    AZTH_SUMMON_COMPANION,
    AZTH_SUMMON_POKEMON_COMPANION,
    AZTH_SUMMON_VEHICLE_MOUNT,
    AZTH_SUMMON_VEHICLE_MOUNT_FLY,
    AZTH_SUMMON_VEHICLE_MORPH,
    AZTH_SUMMON_VEHICLE_MOUNT_FOLLOW,
    AZTH_SUMMON_VEHICLE_MOUNT_FOLLOW_FLY,
    AZTH_SUMMON_VEHICLE_MORPH_FOLLOW,
    AZTH_SUMMON_VEHICLE,
    AZTH_SUMMON_VEHICLE_SPAWN,
    AZTH_SUMMON_VEHICLE_FOLLOW,
    AZTH_SUMMON_VEHICLE_GUARDIAN,
    AZTH_SUMMON_END_MINIONS,
};

class azth_summon : public ItemScript {
public:
    azth_summon(char const* name, AzthSummonType summon_type) : ItemScript(name),
        _summon_type(summon_type)
    {
    }

    bool OnUse(Player *player, Item *item, SpellCastTargets const & /*targets*/) override {
        // CUSTOM WORKAROUND: cooldown category of second spell contains the creature id
        _Spell _time=item->GetTemplate()->Spells[0];
        _Spell _npc=item->GetTemplate()->Spells[1];
        float scale=item->GetTemplate()->ArmorDamageModifier;
        uint32 destId = _npc.SpellCategoryCooldown > 0 ? uint32(_npc.SpellCategoryCooldown) : 0;
        uint32 spawnTime = _time.SpellCategoryCooldown >= 0 ? uint32(_time.SpellCategoryCooldown) : 0;

        bool flyingMount = _summon_type == AZTH_SUMMON_MOUNT_FLY || _summon_type == AZTH_SUMMON_VEHICLE_MOUNT_FLY || _summon_type == AZTH_SUMMON_VEHICLE_MOUNT_FOLLOW_FLY;
        bool mount = _summon_type == AZTH_SUMMON_MOUNT || _summon_type == AZTH_SUMMON_VEHICLE_MOUNT || _summon_type == AZTH_SUMMON_VEHICLE_MOUNT_FOLLOW || flyingMount;

        if (!destId) {
            player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
            return true;
        }
        
        if (!scale)
            scale=1;
        
        if (item->GetTemplate()->RequiredLevel > player->getLevel()) {
            player->GetSession()->SendNotification("Player level too low!");
            player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
            return true;
        }

        if (_summon_type != AZTH_SUMMON_COMPANION && _summon_type != AZTH_SUMMON_MORPH && (
            player->GetMap()->IsDungeon() || player->GetMap()->IsRaid() || player->GetMap()->IsBattlegroundOrArena())) {
            player->GetSession()->SendNotification("Can't do it in this place!");
            player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
            return true; // true to stop
        }
        
        if (_summon_type != AZTH_SUMMON_COMPANION && _summon_type != AZTH_SUMMON_MORPH && player->IsInCombat()) {
            player->GetSession()->SendNotification("Can't do while in combat!");
            player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
            return true; // true to stop
        }

        if (player->IsCharmed()) {
            player->GetSession()->SendNotification("Can't do while charmed!");
            player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
            return true;
        }
        
        if (player->GetModelForForm(player->GetShapeshiftForm())) {
            player->GetSession()->SendNotification("Can't do while shapeshifted!");
            player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
            return true;
        }

        /**
         * AZTH_SUMMON_MOUNT
         */
        if (_summon_type == AZTH_SUMMON_MOUNT || _summon_type == AZTH_SUMMON_MOUNT_FLY) {
            CreatureTemplate const* ci = sObjectMgr->GetCreatureTemplate(destId);
            if (!ci)
            {
                player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
                return true;
            }

            if (player->IsMounted()) {
                
                if (player->GetMountID()  == ci->Modelid1 || player->GetMountID()  == ci->Modelid2 ||
                player->GetMountID()  == ci->Modelid3 || player->GetMountID()  == ci->Modelid4) {
                    player->Dismount();
                    player->CastSpell(player, 53708, TRIGGERED_FULL_MASK); // visual  
                    player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
                    return true;
                } 
                // else {
                //      do nothing and go ahead with the mount check below
                // }
  
            } else {
                /*if (!player->IsOutdoors()) {
                    player->GetSession()->SendNotification("Can't do indoor!");
                    player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
                    return true;
                }*/
                
                uint32 displayID = ObjectMgr::ChooseDisplayId(ci);
                sObjectMgr->GetCreatureModelRandomGender(&displayID);

                player->Mount(displayID, ci->VehicleId, destId);
                
                uint32 spell = flyingMount ? 1002002 : 1002001;
                player->CastSpell(player, spell , TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE);

                if (!player->HasAura(spell)) {
                    player->Dismount();
                    player->GetSession()->SendNotification("You can't mount now!");
                    player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
                    return true; 
                }
                
                player->CastSpell(player, 53708, TRIGGERED_FULL_MASK); // visual  

                return false;
            }
        }
        
        if (player->IsMounted()) {
            player->GetSession()->SendNotification("Can't do while mounted!");
            player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
            return true;
        }

        /**
         * AZTH_SUMMON_MORPH
         */
        if (_summon_type == AZTH_SUMMON_MORPH) {
            if (player->GetDisplayId() == destId) {
                player->DeMorph();
                player->SetObjectScale(1);
                player->CastSpell(player, 53708, TRIGGERED_FULL_MASK); // visual   
                player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
                return true;
            } else {
                player->SetDisplayId(destId);
                player->SetObjectScale(scale);
                player->CastSpell(player, 53708, TRIGGERED_FULL_MASK); // visual   
                return false;
            }
        }
        
        if (_summon_type >= AZTH_SUMMON_START_MINIONS && _summon_type <= AZTH_SUMMON_END_MINIONS ) {

            if (uint64 pet_guid = player->GetCritterGUID())
            {
                if (Creature* pet = ObjectAccessor::GetCreatureOrPetOrVehicle(*player, pet_guid)) {
                    if (destId == pet->GetEntry())
                    {
                        if (pet->GetTypeId() == TYPEID_UNIT && pet->ToCreature()->IsSummon())
                            pet->ToTempSummon()->UnSummon();
                        
                        player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
                        return true;
                    }
                }
            }
 
            // 161 -> category 4 : SUMMON_CATEGORY_VEHICLE
            // 65  -> category 3 : SUMMON_CATEGORY_PUPPET (it's controllable. you can move with keyboard)
            // 41  -> category 1 : SUMMON_CATEGORY_ALLY  + TYPE 5 + SLOT 5 (critter/minipet)
            // category 2: SUMMON_CATEGORY_PET
            uint32 propId = _summon_type >= AZTH_SUMMON_VEHICLE_MOUNT && _summon_type <= AZTH_SUMMON_VEHICLE_GUARDIAN ? 161 : 41;
            SummonPropertiesEntry const *properties = sSummonPropertiesStore.LookupEntry(propId);
            Minion* summon = (Minion*)player->GetMap()->SummonCreature(destId, player->GetWorldLocation(), properties, spawnTime, player);

            if (!summon) {
                player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
                return true;
            }
            
            //player->SetMinion(summon, true);

            summon->SelectLevel();       // some summoned creaters have different from 1 DB data for level/hp
            // xinef: set calculated level
            summon->SetLevel(player->getLevel());

            summon->setFaction(player->getFaction());
            summon->SetOwnerGUID(player->GetGUID());
            summon->SetCreatorGUID(player->GetGUID());                
            summon->SetObjectScale(scale);
            
            Creature* creature = ObjectAccessor::GetCreatureOrPetOrVehicle(*player, summon->GetGUID());
            if (creature) {
                creature->CastSpell(creature, 53708, TRIGGERED_FULL_MASK);
            }
            
            if (_summon_type == AZTH_SUMMON_COMPANION) { 
                summon->SetUInt32Value(UNIT_NPC_FLAGS, summon->GetCreatureTemplate()->npcflag);

                summon->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                summon->SetReactState(REACT_PASSIVE);
                player->SetCritterGUID(creature->GetGUID());
            }

            if (_summon_type == AZTH_SUMMON_VEHICLE || _summon_type == AZTH_SUMMON_VEHICLE_MORPH 
                || _summon_type == AZTH_SUMMON_VEHICLE_MOUNT || _summon_type == AZTH_SUMMON_VEHICLE_SPAWN) {
                // enter
                if (creature && _summon_type != AZTH_SUMMON_VEHICLE_SPAWN) {
                    player->EnterVehicle(creature, 0);
                }
            } else {
                // follow
                if (!summon->IsInCombat() && !summon->IsTrigger()) {
                    // xinef: move this here, some auras are added in initstatsforlevel!
                    summon->GetMotionMaster()->Clear(false);
                    summon->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, summon->GetFollowAngle(), MOTION_SLOT_ACTIVE);
                    //summon->SetSpeed(MOVE_RUN, scale);
                    //summon->SetSpeed(MOVE_WALK, scale);
                }
            }
            
            return false; // continue with item process (spells)
        }
        

        if (_summon_type == AZTH_SUMMON_GUARDIAN) {
            TempSummon* petSummon = player->SummonCreature(destId, player->GetPositionX()+1, player->GetPositionY()+1, player->GetPositionZ(), player->GetOrientation()
                                                , TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, spawnTime);
            
            if (!petSummon) {
                player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
                return true;
            }

            Pet* pet = player->CreateTamedPetFrom(petSummon, 0);
            if (!pet) {
                player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
                return true;
            }

            pet->SetPower(POWER_HAPPINESS, 100);
            pet->AddUInt64Value(UNIT_FIELD_CREATEDBY, player->GetGUID());
            pet->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, player->getFaction());
            pet->SetUInt32Value(UNIT_FIELD_LEVEL, player->getLevel());
            pet->SetObjectScale(scale);
            pet->GetMap()->AddToMap(pet->ToCreature());
            pet->GetCharmInfo()->SetPetNumber(sObjectMgr->GeneratePetNumber(), true);
            player->SetMinion(pet, true);
            pet->SavePetToDB(PET_SAVE_AS_CURRENT, false);
            pet->InitTalentForLevel();
            pet->InitStatsForLevel(player->getLevel());
            player->PetSpellInitialize();

            petSummon->DespawnOrUnsummon(100); // remove temp summoned creature
            
            pet->CastSpell(pet, 53708, TRIGGERED_FULL_MASK); // visual   
            return false; // continue with item process (spells)
        }
        
        return false; // continue with item process (spells)
    }
    
private:
    
    AzthSummonType _summon_type;
};

class azth_mass_ress : public ItemScript {
public:
    azth_mass_ress(char const* name) : ItemScript(name)
    {
    }

    bool OnUse(Player *player, Item *item, SpellCastTargets const & /*targets*/) override {
        if (player->IsInCombat() || 
            (player->GetInstanceScript() && player->GetInstanceScript()->IsEncounterInProgress()))
        {
            player->SendEquipError(EQUIP_ERR_NOT_IN_COMBAT, item, NULL);
            return true;
        }
        
        if (Group* group = player->ToPlayer()->GetGroup()) {
            for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next()) {
                if (Player* member = itr->GetSource()) {
                    if (member->IsInMap(player) && member->IsInCombat()) {
                        player->GetSession()->SendNotification("%s",sAzthLang->get(AZTH_LANG_RAID_COMBAT, player));
                        player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
                        return true; 
                    }
                }
            }
        }
        
        if (!player->GetMap()->IsDungeon()) {
            player->GetSession()->SendNotification("%s",sAzthLang->get(AZTH_LANG_INSTANCE_ONLY, player));
            player->SendEquipError(EQUIP_ERR_NONE, item, NULL);
            return true;
        }
        
        if (item->GetSpellCharges()) // if charges remains then can cast spell
            player->CastSpell(player, 72429, TRIGGERED_FULL_MASK); // visual but also works when player not released
        
        return false; // everything ok
    }
};

void AddSC_azth_extra_items() // Add to scriptloader normally
{
    // vanity
    new azth_summon("azth_summon_morph", AZTH_SUMMON_MORPH);
    new azth_summon("azth_summon_mount", AZTH_SUMMON_MOUNT);
    new azth_summon("azth_summon_mount_fly", AZTH_SUMMON_MOUNT_FLY);
    new azth_summon("azth_summon_companion", AZTH_SUMMON_COMPANION);
    new azth_summon("azth_summon_guardian", AZTH_SUMMON_GUARDIAN);
    new azth_summon("azth_summon_vehicle_mount", AZTH_SUMMON_VEHICLE_MOUNT);
    new azth_summon("azth_summon_vehicle_mount_fly", AZTH_SUMMON_VEHICLE_MOUNT_FLY);
    new azth_summon("azth_summon_vehicle_morph", AZTH_SUMMON_VEHICLE_MORPH);
    new azth_summon("azth_summon_vehicle_mount_follow", AZTH_SUMMON_VEHICLE_MOUNT_FOLLOW);
    new azth_summon("azth_summon_vehicle_mount_follow_fly", AZTH_SUMMON_VEHICLE_MOUNT_FOLLOW_FLY);
    new azth_summon("azth_summon_vehicle_morph_follow", AZTH_SUMMON_VEHICLE_MORPH_FOLLOW);
    new azth_summon("azth_summon_vehicle", AZTH_SUMMON_VEHICLE);
    new azth_summon("azth_summon_vehicle_follow", AZTH_SUMMON_VEHICLE_FOLLOW);
    new azth_summon("azth_summon_vehicle_guardian", AZTH_SUMMON_VEHICLE_GUARDIAN);
    // tools
    new azth_mass_ress("azth_tools_mass_ress");
}