#ifndef ZFROSTEFFECT_H
#define ZFROSTEFFECT_H

#include "types.h"

class zFrostIcicle
{
public:
    struct config
    {
        F32 areaPerIcicle;
        F32 lifetime;
        F32 maxScale;
        F32 minScale;
        F32 growTime;
        F32 shrinkTime;
    };
};

#endif
