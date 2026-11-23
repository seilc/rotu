#ifndef ZUICUSTOMCHECKPOINTS_H
#define ZUICUSTOMCHECKPOINTS_H

#include "zUICustom.h"

struct RwTexture;

void zUIIncrediblesSetSceneCheckPointHit(U32 value);
void zUIIncrediblesSetSceneCheckPointTotal(U32 value);
U32 zUIIncrediblesGetSceneCheckPointHit();
U32 zUIIncrediblesGetSceneCheckPointTotal();

class zUICustomCheckpoints : public zUICustom
{
public:
    zUICustomCheckpoints(zUI* ui);

    virtual void Reset();
    virtual bool PreRender();

private:
    RwTexture* texture;
};

#endif
