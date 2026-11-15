#include "xCommon.h"
#include "zUICustomGenericHUD.h"

#include "xEvent.h"
#include "xIni.h"
#include "xModel.h"
#include "zEvent.h"
#include "zScene.h"
#include "zUIModel.h"
#include "zUIUVs.h"

#include <stdlib.h>

namespace {

struct zUIModelUVs
{
    U32 id;
    F32* offsets;
};

zUIModelUVs* uvs;
S32 uvsCount;
S32 uvsIdx;

S32 zUIModelUVs_Find(U32 id)
{
    for (S32 i = 0; i < uvsCount; i++) {
        if (uvs[i].id == id) {
            return i;
        }
    }

    return -1;
}

} // namespace

zUICustomGenericHUDManager* pUICustomGenericHUDManager;

void zUIModelUVs_Init(S32 count)
{
    uvs = (zUIModelUVs*)xMEMALLOC(count * sizeof(zUIModelUVs), 0);
    uvsCount = count;
}

void zUIModelUVs_Init(U32 id, xIniValue* values, S32 count)
{
    zUIModelUVs* uv = &uvs[uvsIdx];

    uv->id = id;
    uv->offsets = (F32*)xMEMALLOC(count * sizeof(F32), 0);

    for (S32 i = 0; i < count; i++) {
        uv->offsets[i] = atof(values[i].val);
    }

    uvsIdx++;
}

zUICustomGenericHUDManager::zUICustomGenericHUDManager(zUI* ui) : zUICustom(ui)
{
    pUICustomGenericHUDManager = this;
    visible = false;
}

void zUICustomGenericHUDManager::Init()
{
}

void zUICustomGenericHUDManager::ShiftUVs(U32 id, bool forward)
{
    S32 count = get_size_queue();

    if (count > 0) {
        zUIGenericHUDModel hudModel;
        S32 i;

        for (i = 0; i < count; i++) {
            fetch_queue_copy_offset(hudModel, i);
            if (hudModel.model->id == id) {
                break;
            }
        }

        zUIGenericHUDModel* entry = fetch_queue_ptr_offset(i);

        if (forward) {
            entry->offset++;
        } else if (entry->offset > 0) {
            entry->offset--;
        }

        ResetUVs(i);
    }
}

bool zUICustomGenericHUDManager::HandleEvent(xBase*, U32 toEvent, const F32* toParam, xBase*, U32 toParamWidgetID)
{
    switch (toEvent) {
    case eEventReset:
        ResetUVs();
        break;
    case eEventUIModelBarOffsetNext:
        ShiftUVs(toParamWidgetID, true);
        break;
    case eEventUIModelBarOffsetPrevious:
        // BUG: true, not false
        ShiftUVs(toParamWidgetID, true);
        break;
    case eEventVisible:
        if (toParam[0]) {
            zUIGenericHUDModel hudModel;

            hudModel.model = (zUIModel*)zSceneFindObject(*(const U32*)toParam);
            hudModel.offset = 0;
            hudModel.uvsIdx = zUIModelUVs_Find(hudModel.model->id);

            S32 index = add_queue(hudModel);
            ResetUVs(index);
            visible = true;
        }
        break;
    case eEventInvisible:
        if (toParam[0]) {
            xBase* obj = zSceneFindObject(*(const U32*)toParam);
            S32 count = get_size_queue();
            zUIGenericHUDModel hudModel;

            for (S32 i = 0; i < count; i++) {
                fetch_queue_copy_offset(hudModel, i);
                if (hudModel.model->id == obj->id) {
                    ResetUVs(i);
                    get_queue(hudModel, i);
                    break;
                }
            }

            visible = false;
        }
        break;
    }

    return true;
}

void zUICustomGenericHUDManager::ResetUVs()
{
    S32 count = get_size_queue();

    for (S32 i = 0; i < count; i++) {
        ResetUVs(i);
        ResetUVsOffset(i);
    }
}

void zUICustomGenericHUDManager::ResetUVs(S32 index)
{
    zUIGenericHUDModel hudModel;

    fetch_queue_copy_offset(hudModel, index);

    zUIModel* model = hudModel.model;
    zUIUVs_Reset(model->GetModel()->Data, model->GetUVs());
}

void zUICustomGenericHUDManager::ResetUVsOffset(S32 index)
{
    zUIGenericHUDModel* entry = fetch_queue_ptr_offset(index);
    entry->offset = 0;
}

void zUICustomGenericHUDManager::SetRatio(F32 ratio, S32 index, bool decrease)
{
    zUIGenericHUDModel hudModel;

    fetch_queue_copy_offset(hudModel, index);

    F32 offset_u = uvs[hudModel.uvsIdx].offsets[hudModel.offset];
    zUIModel* model = hudModel.model;
    zUIUVs_Update(model->GetModel()->Data, ratio, model->GetUVs(), false, true, 1, offset_u);

    if (!decrease) {
        zEntEvent(ui, eEventUIHUDGenericIncrease);
    } else {
        zEntEvent(ui, eEventUIHUDGenericDecrease);
    }
}
