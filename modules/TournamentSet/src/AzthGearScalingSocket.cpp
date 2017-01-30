#include "AzthGearScalingSocket.h"

AzthGearScalingSocket::AzthGearScalingSocket()
{
    itemId = uint32(80);
    socket1 = uint32(0);
    socket2 = uint32(0);
    socket3 = uint32(0);
    enchant = uint32(0);
}

AzthGearScalingSocket::AzthGearScalingSocket(uint32 itemId, uint32 socket1, uint32 socket2, uint32 socket3, uint32 enchant)
{
    this->itemId = itemId;
    this->socket1 = socket1;
    this->socket2 = socket2;
    this->socket3 = socket3;
    this->enchant = enchant;
}

map<uint32, AzthGearScalingSocket> AzthGearScalingSocket::GetGearScalingSocketList() const
{
    return gearScalingSocketList;
}

void AzthGearScalingSocket::SetGearScalingSocketList(map<uint32, AzthGearScalingSocket> gearScalingSocketList)
{
    this->gearScalingSocketList = gearScalingSocketList;
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