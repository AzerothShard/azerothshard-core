#include "AzthGearScaling.h"

AzthGearScaling::AzthGearScaling()
{
    season = uint32(80);
    Class = uint32(0);
    spec = uint32(0);
    name = string("");
    head = uint32(0);
    neck = uint32(0);
    shoulders = uint32(0);
    body = uint32(0);
    chest = uint32(0);
    waist = uint32(0);
    legs = uint32(0);
    feet = uint32(0);
    wrists = uint32(0);
    wrists_h = uint32(0);
    finger1 = uint32(0);
    finger1_h = uint32(0);
    finger2 = uint32(0);
    finger2_h = uint32(0);
    trinket1 = uint32(0);
    trinket1_h = uint32(0);
    trinket2 = uint32(0);
    trinket2_h = uint32(0);
    back = uint32(0);
    mainHand = uint32(0);
    offHand = uint32(0);
    ranged = uint32(0);
    tabard = uint32(0);
}

AzthGearScaling::AzthGearScaling(uint32 season, uint32 Class, uint32 spec, string name, uint32 head, uint32 neck, uint32 shoulders, uint32 body, uint32 chest, uint32 waist, uint32 legs, uint32 feet, uint32 wrists, uint32 wrists_h, uint32 hands, uint32 finger1, uint32 finger1_h, uint32 finger2, uint32 finger2_h, uint32 trinket1, uint32 trinket1_h, uint32 trinket2, uint32 trinket2_h, uint32 back, uint32 mainHand, uint32 offHand, uint32 ranged, uint32 tabard)
{
    this->season = season;
    this->Class = Class;
    this->spec = spec;
    this->name = name;
    this->head = head;
    this->neck = neck;
    this->shoulders = shoulders;
    this->body = body;
    this->chest = chest;
    this->waist = waist;
    this->legs = legs;
    this->feet = feet;
    this->wrists = wrists;
    this->wrists_h = wrists_h;
    this->hands = hands;
    this->finger1 = finger1;
    this->finger1_h = finger1_h;
    this->finger2 = finger2;
    this->finger2_h = finger2_h;
    this->trinket1 = trinket1;
    this->trinket1_h = trinket1_h;
    this->trinket2 = trinket2;
    this->trinket2_h = trinket2_h;
    this->back = back;
    this->mainHand = mainHand;
    this->offHand = offHand;
    this->ranged = ranged;
    this->tabard = tabard;
}

map<uint32, AzthGearScaling> AzthGearScaling::GetGearScalingList() const
{
    return gearScalingList;
}

void AzthGearScaling::SetGearScalingList(map<uint32, AzthGearScaling> gearScalingList)
{
    this->gearScalingList = gearScalingList;
}

uint32 AzthGearScaling::GetSeason() const
{
    return season;
}

uint32 AzthGearScaling::GetClass() const
{
    return Class;
}

uint32 AzthGearScaling::GetSpec() const
{
    return spec;
}

string AzthGearScaling::GetName() const
{
    return name;
}

uint32 AzthGearScaling::GetHead() const
{
    return head;
}

uint32 AzthGearScaling::GetNeck() const
{
    return neck;
}

uint32 AzthGearScaling::GetShoulders() const
{
    return shoulders;
}

uint32 AzthGearScaling::GetBody() const
{
    return body;
}

uint32 AzthGearScaling::GetChest() const
{
    return chest;
}

uint32 AzthGearScaling::GetWaist() const
{
    return waist;
}

uint32 AzthGearScaling::GetLegs() const
{
    return legs;
}

uint32 AzthGearScaling::GetFeet() const
{
    return feet;
}

uint32 AzthGearScaling::GetWrists() const
{
    return wrists;
}

uint32 AzthGearScaling::GetWrists_h() const
{
    return wrists_h;
}

uint32 AzthGearScaling::GetHands() const
{
    return hands;
}

uint32 AzthGearScaling::GetFinger1() const
{
    return finger1;
}

uint32 AzthGearScaling::GetFinger1_h() const
{
    return finger1_h;
}

uint32 AzthGearScaling::GetFinger2() const
{
    return finger2;
}

uint32 AzthGearScaling::GetFinger2_h() const
{
    return finger2_h;
}

uint32 AzthGearScaling::GetTrinket1() const
{
    return trinket1;
}

uint32 AzthGearScaling::GetTrinket1_h() const
{
    return trinket1_h;
}

uint32 AzthGearScaling::GetTrinket2() const
{
    return trinket2;
}

uint32 AzthGearScaling::GetTrinket2_h() const
{
    return trinket2_h;
}

uint32 AzthGearScaling::GetBack() const
{
    return back;
}

uint32 AzthGearScaling::GetMainHand() const
{
    return mainHand;
}

uint32 AzthGearScaling::GetOffHand() const
{
    return offHand;
}

uint32 AzthGearScaling::GetRanged() const
{
    return ranged;
}


uint32 AzthGearScaling::GetTabard() const
{
    return tabard;
}
