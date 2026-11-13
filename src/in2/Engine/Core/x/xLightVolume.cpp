#include "xCommon.h"
#include "xLightVolume.h"

#include "xMemMgr.h"
#include "iModel.h"

#define MAX_ACTIVE 64

struct xLightVolume::activity_type
{
    xLightVolume* owner;
};

xLightVolume::activity_type* xLightVolume::activities = NULL;
S32 xLightVolume::activities_used = 0;

void xLightVolume::scene_enter()
{
    activities = new (eMemStaticType_0, eMemMgrTag_Light, 0) activity_type[MAX_ACTIVE];
    activities_used = 0;
    reset_all();
}

void xLightVolume::scene_exit()
{
}

void xLightVolume::reset_all()
{
    activity_type* act = activities, *end_act = act + activities_used;

    for (; act != end_act; act++) {
        act->owner->activity = 0;
    }

    activities_used = 0;
}

void xLightVolume::create()
{
    activity = 0;
}

bool xLightVolume::activate()
{
    if (activities_used >= MAX_ACTIVE) {
        return false;
    }

    activity = &activities[activities_used];
    activities_used++;
    activity->owner = this;

    return true;
}

void xLightVolume::deactivate()
{
    activities_used--;

    activity_type* tail_activity = &activities[activities_used];

    if (activity != tail_activity) {
        *activity = *tail_activity;
        activity->owner->activity = activity;
    }

    activity = 0;
}

void xLightVolume::render_all()
{
}

void xLightVolume::render_atomic(RpAtomic*, xColor, bool, U32)
{
}
