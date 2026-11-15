#ifndef ZUIMODEL_H
#define ZUIMODEL_H

#include "zUI.h"

struct xModelInstance;
struct xAnimTable;
struct xSurface;

class zUIModel : public zUI
{
public:
    xModelInstance* GetModel() const
    {
        return model;
    }

    F32* GetUVs() const
    {
        return uvCoords;
    }

private:
    xModelInstance* model;
    xAnimTable* atbl;
    xSurface* surf;
    F32* uvCoords;
    U32 uvCoordsCount;
};

#endif
