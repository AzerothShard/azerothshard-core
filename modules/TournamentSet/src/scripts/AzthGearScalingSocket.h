#include "Common.h"
#include "Define.h"
#include "Config.h"

class AzthGearScalingSocket
{
    friend class ACE_Singleton<AzthGearScalingSocket, ACE_Null_Mutex>;
public:
    //GETTERS 
    uint32 GetItemId() const;
    uint8 GetClass() const;
    uint8 GetSpec() const;
    uint32 GetSocket1() const;
    uint32 GetSocket2() const;
    uint32 GetSocket3() const;
    uint32 GetEnchant() const;

    map<uint64, AzthGearScalingSocket> GetGearScalingSocketList() const;

    void SetGearScalingSocketList(map<uint64, AzthGearScalingSocket> gearScalingSocketList);
    AzthGearScalingSocket();
    AzthGearScalingSocket(uint32 itemId, uint8 plClass, uint8 spec, uint32 socket1, uint32 socket2, uint32 socket3, uint32 enchant);


private:
    uint32 itemId;
    uint8 plClass;
    uint8 spec;
    uint32 socket1;
    uint32 socket2;
    uint32 socket3;
    uint32 enchant;
    map<uint64, AzthGearScalingSocket> gearScalingSocketList;
};

#define sAzthGearScalingSocket ACE_Singleton<AzthGearScalingSocket, ACE_Null_Mutex>::instance()