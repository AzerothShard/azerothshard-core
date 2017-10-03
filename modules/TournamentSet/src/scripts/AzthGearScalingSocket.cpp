#include "AzthGearScalingSocket.h"

AzthGearScalingSocket::AzthGearScalingSocket()
{
    itemId  = uint32(0);
    plClass   = uint8(0);
    spec    = uint8(0);
    socket1 = uint32(0);
    socket2 = uint32(0);
    socket3 = uint32(0);
    enchant = uint32(0);
}

AzthGearScalingSocket::AzthGearScalingSocket(uint32 itemId, uint8 plClass, uint8 spec, uint32 socket1, uint32 socket2, uint32 socket3, uint32 enchant)
{
    this->itemId = itemId;
    this->plClass = plClass;
    this->spec = spec;
    this->socket1 = socket1;
    this->socket2 = socket2;
    this->socket3 = socket3;
    this->enchant = enchant;
}

map<uint64, AzthGearScalingSocket> AzthGearScalingSocket::GetGearScalingSocketList() const
{
    return gearScalingSocketList;
}

void AzthGearScalingSocket::SetGearScalingSocketList(map<uint64, AzthGearScalingSocket> gearScalingSocketList)
{
    this->gearScalingSocketList = gearScalingSocketList;
}

uint8 AzthGearScalingSocket::GetClass() const
{
    return plClass;
}

uint8 AzthGearScalingSocket::GetSpec() const
{
    return spec;
}

uint32 AzthGearScalingSocket::GetItemId() const
{
    return itemId;
}

uint32 AzthGearScalingSocket::GetSocket1() const
{
    return socket1;
}

uint32 AzthGearScalingSocket::GetSocket2() const
{
    return socket2;
}

uint32 AzthGearScalingSocket::GetSocket3() const
{
    return socket3;
}

uint32 AzthGearScalingSocket::GetEnchant() const
{
    return enchant;
}