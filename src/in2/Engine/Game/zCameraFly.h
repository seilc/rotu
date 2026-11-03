#ifndef ZCAMERAFLY_H
#define ZCAMERAFLY_H

#include "types.h"

struct zFlyKey
{
    S32 frame;
    F32 matrix[12];
    F32 aperture[2];
    F32 focal;
};

#endif
