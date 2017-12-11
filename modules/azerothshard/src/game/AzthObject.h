#ifndef AZTHOBJECT_H
#define AZTHOBJECT_H

#include "Config.h"
#include "Define.h"
#include <UnorderedMap.h>

enum EUint32Fields {
    AZTH_U32FIELD_PLAYER_EXTRA_MOUNT_STATUS,
};

enum EFloatFields {
};

class Object;


class AzthObject {
public:
    explicit AzthObject(Object *origin);
    ~AzthObject();
    
    void setBoolValue(uint32 index, bool value) {
        m_dataUint32[index] = value ? 1 : 0;
    }

    bool getBoolValue(uint32 index) {
        return m_dataUint32[index] > 0;
    }

    void setUint32Value(uint32 index, uint32 value) {
        m_dataUint32[index] = value;
    }

    uint32 getUint32Value(uint32 index) {
        return m_dataUint32[index];
    }

    void setFloatValue(uint32 index, float value) {
        m_dataFloat[index] = value;
    }

    float getfloatValue(uint32 index) {
        return m_dataFloat[index];
    }


    typedef UNORDERED_MAP<uint32 /*index*/, uint32 /*value*/>
        DataUint32;
    DataUint32 m_dataUint32;

    typedef UNORDERED_MAP<uint32 /*index*/, float /*value*/>
        DataFloat;
    DataFloat m_dataFloat;
};

#endif /* AZTHOBJECT_H */
