#ifndef ZUICUSTOMGENERICHUD_H
#define ZUICUSTOMGENERICHUD_H

#include "zQueue.h"
#include "zUICustom.h"

class zUIModel;
struct xIniValue;

struct zUIGenericHUDModel
{
    zUIModel* model;
    S32 offset;
    S32 uvsIdx;

    zUIGenericHUDModel& operator=(const zUIGenericHUDModel& other)
    {
        model = other.model;
        offset = other.offset;
        uvsIdx = other.uvsIdx;
        return *this;
    }
};

class zUICustomGenericHUDManager : public zUICustom, public zQueue<zUIGenericHUDModel, 64>
{
public:
    zUICustomGenericHUDManager(zUI* ui);

    virtual void Init();
    virtual bool HandleEvent(xBase* from, U32 toEvent, const F32* toParam, xBase* toParamWidget, U32 toParamWidgetID);

    void ShiftUVs(U32 id, bool forward);
    void ResetUVs();
    void ResetUVs(S32 index);
    void ResetUVsOffset(S32 index);
    void SetRatio(F32 ratio, S32 index, bool decrease);

private:
    bool visible;
};

extern zUICustomGenericHUDManager* pUICustomGenericHUDManager;

void zUIModelUVs_Init(S32 count);
void zUIModelUVs_Init(U32 id, xIniValue* values, S32 count);

#endif
