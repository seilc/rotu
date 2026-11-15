#ifndef ZNPCBEHAVIOR_H
#define ZNPCBEHAVIOR_H

#include "zBehavior.h"
#include "zNPCCommon.h"

namespace zNPC {

class move : public zBehaviorImplementation<zNPCCommon>
{
public:
    virtual const char* getName() { return "move"; }
    virtual void setup();
    virtual void reset();

    F32 get_angle_difference(const xVec3*, const xVec3*) const;
    F32 apply_angle_diff(F32 angle_difference, F32 dt, F32 turning_speed);

protected:
    F32 turn_speed;
    F32 move_speed;
    F32 min_speed;
    F32 acceleration;
    F32 speed;
};

}

#endif
