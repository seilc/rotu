#ifndef ZNPCPROJECTILEGRUB_H
#define ZNPCPROJECTILEGRUB_H

#include "zNPCProjectileGrubFX.h"

struct xEnt;

struct GrubConfig
{
    xEnt* owner;
    xVec3 startPos;
    xVec3 dir;
    S32 damage;
    F32 speed;
};

extern zNPCProjectileGrubArc::ArcConfig zNPCProjectileGrubArcPlayerConfig;
extern zNPCProjectileGrubArc::ArcConfig zNPCProjectileGrubArcNPCConfig;

#endif
