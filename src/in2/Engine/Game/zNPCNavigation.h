#ifndef ZNPCNAVIGATION_H
#define ZNPCNAVIGATION_H

#include "xVec3.h"

class zNPCNavigate
{
public:
    bool GetClosestPosition(const xVec3&, xVec3&);

    void dont_apply_dpos() { dpos_apply = false; }

    F32 stuck_timer;
    bool dpos_apply;
};

#endif
