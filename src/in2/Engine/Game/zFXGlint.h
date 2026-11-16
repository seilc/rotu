#ifndef ZFXGLINT_H
#define ZFXGLINT_H

#include "types.h"

struct xVec3;

namespace zFX
{

class glint
{
public:
    struct config
    {
        F32 min_life;
        F32 max_life;
        F32 min_size;
        F32 max_size;
        F32 emit_rate;
        F32 fade_in;
        F32 fade_out;
        U8 r;
        U8 g;
        U8 b;
        U8 cinematic_only;
        F32 size_delta;
    };

    static void setup();
    static void emit(const xVec3& position, F32& emit_remaining, F32 dt, config* cfg);

    static config default_config;
};

} // namespace zFX

#endif
