#ifndef ZFXSCREENDECAL_H
#define ZFXSCREENDECAL_H

#include "xMath3.h"

#include <rwcore.h>

class zFXScreenDecalConfig
{
public:
    F32 fadeInTime;
    F32 fadeOutTime;
    F32 lifetime;
    xFRect screenSpace;
    U32 textureID;
    RwRaster* raster;
    U8 alpha;

    void SetRasterFromID();
};

void zFXScreenDecalSceneEnter();
void zFXScreenDecalSceneReset();
void zFXScreenDecalUpdate(F32 dt);
bool zFXScreenDecalStart(const zFXScreenDecalConfig* config);
void zFXScreenDecalRender();

#endif
