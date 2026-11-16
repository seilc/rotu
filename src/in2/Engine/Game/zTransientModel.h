#ifndef ZTRANSIENTMODEL_H
#define ZTRANSIENTMODEL_H

#include "xColor.h"
#include "xMath3.h"

class zTransientModelEmitter
{
public:
    struct config_type
    {
        S32 max_active;
        S32 overlap_max;
        F32 overlap_radius;
        F32 kill_dist;
        F32 fade_in_time;
        F32 fade_out_time;
        F32 max_life;
        F32 scale_min;
        F32 scale_max;
        F32 intensity_min;
        F32 intensity_max;
        xFColor color;
        const U32* models;
        S32 models_size;
    };

    struct activity_data;

    void create();
    void reset();
    void activate(const config_type& cfg);
    void set_config(const config_type& cfg);
    void spawn(const xMat4x3& mat, F32 fade_in_time, U32 flags);

    activity_data* act;
};

#endif
