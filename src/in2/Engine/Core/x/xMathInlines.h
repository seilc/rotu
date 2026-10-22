#ifndef XMATHINLINES_H
#define XMATHINLINES_H

#if DEBUG
#define inline
#endif

inline F32 xatan(F32 x)
{
    return std::atanf(x);
}

#undef inline

#endif
