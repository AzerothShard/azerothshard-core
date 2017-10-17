#include "Common.h"
#include "Define.h"
#include "Config.h"

class AzthGearScaling
{
    friend class ACE_Singleton<AzthGearScaling, ACE_Null_Mutex>;
public:
    //GETTERS 
    uint32 GetSeason() const;
    uint32 GetClass() const;
    uint32 GetSpec() const;
    string GetName() const;
    uint32 GetHead() const;
    uint32 GetNeck() const;
    uint32 GetShoulders() const;
    uint32 GetBody() const;
    uint32 GetChest() const;
    uint32 GetWaist() const;
    uint32 GetLegs() const;
    uint32 GetFeet() const;
    uint32 GetHands() const;
    uint32 GetWrists() const;
    uint32 GetWrists_h() const;
    uint32 GetFinger1() const;
    uint32 GetFinger1_h() const;
    uint32 GetFinger2_h() const;
    uint32 GetFinger2() const;
    uint32 GetTrinket1() const;
    uint32 GetTrinket1_h() const;
    uint32 GetTrinket2() const;
    uint32 GetTrinket2_h() const;
    uint32 GetBack() const;
    uint32 GetMainHand() const;
    uint32 GetOffHand() const;
    uint32 GetRanged() const;
    uint32 GetTabard() const;

    AzthGearScaling();
    AzthGearScaling(uint32 season, uint32 Class, uint32 spec, string name, uint32 head, uint32 neck, uint32 shoulders, uint32 body, uint32 chest, uint32 waist, uint32 legs, uint32 feet, uint32 wrists, uint32 wrists_h, uint32 hands, uint32 finger1, uint32 finger1_h, uint32 finger2, uint32 finger2_h, uint32 trinket1, uint32 trinket1_h, uint32 trinket2, uint32 trinket2_h, uint32 back, uint32 mainHand, uint32 offHand, uint32 ranged, uint32 tabard);

private:
    string name;
    uint32 head;
    uint32 neck;
    uint32 shoulders;
    uint32 body;
    uint32 chest;
    uint32 waist;
    uint32 legs;
    uint32 feet;
    uint32 wrists;
    uint32 wrists_h;
    uint32 hands;
    uint32 finger1;
    uint32 finger1_h;
    uint32 finger2;
    uint32 finger2_h;
    uint32 trinket1;
    uint32 trinket1_h;
    uint32 trinket2;
    uint32 trinket2_h;
    uint32 back;
    uint32 mainHand;
    uint32 offHand;
    uint32 ranged;
    uint32 tabard;
    uint32 spec;
    uint32 season;
    uint32 Class;

};

class AzthGearScalingMgr {
public:    
    map<uint32, AzthGearScaling> & GetGearScalingList();
    void SetGearScalingList(map<uint32, AzthGearScaling> gearScalingList);   
private:
    map<uint32, AzthGearScaling> gearScalingList;
};

#define sAzthGearScalingMgr ACE_Singleton<AzthGearScalingMgr, ACE_Null_Mutex>::instance()