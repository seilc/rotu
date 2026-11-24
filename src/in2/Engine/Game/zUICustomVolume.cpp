#include "xCommon.h"
#include "zUICustomVolume.h"

#include "xGroup.h"
#include "xSndMgr.h"
#include "zEvent.h"
#include "zScene.h"
#include "zUI.h"
#include "zUIImage.h"

zUICustomVolume* pUICustomMusicVolumeManager;
zUICustomVolume* pUICustomSFXVolumeManager;

zUICustomVolume::zUICustomVolume(zUI* ui, xSndMixerFaderName sndMixerFader) : zUICustom(ui), sndMixerFader(sndMixerFader)
{
}

void zUICustomVolume::Setup()
{
    xGroup* group = (xGroup*)zSceneFindObject(ui->asset->customWidget);
    if (group == NULL) {
        return;
    }

    numItems = 0;
    for (U32 i = 0; i < xGroupGetCount(group); i++) {
        xBase* b = xGroupGetItemPtr(group, i);
        if (b)
            numItems++;
    }

    if (numItems == 0) {
        return;
    }

    bars = (zUIImage**)zUIAllocStaticMemory(numItems * sizeof(zUIImage*));

    S32 itemIterator = 0;
    for (U32 i = 0; i < xGroupGetCount(group); i++) {
        xBase* b = xGroupGetItemPtr(group, i);
        if (b)
            bars[itemIterator++] = (zUIImage*)b;
    }
}

bool zUICustomVolume::PreRender()
{
    return false;
}

void zUICustomVolume::RefreshBars()
{
    if (bars == NULL) {
        return;
    }

    S32 volume = GetVolume() * 11.0f + 0.25f;

    for (S32 i = 0; i < numItems; i++) {
        bars[i]->HandleEvent(NULL, i < volume ? eEventVisible : eEventInvisible, NULL, NULL, 0);
    }
}

void zUICustomVolume::HideBars()
{
    if (bars == NULL) {
        return;
    }

    for (S32 i = 0; i < numItems; i++) {
        bars[i]->HandleEvent(NULL, eEventInvisible, NULL, NULL, 0);
    }
}

bool zUICustomVolume::HandleEvent(xBase*, U32 toEvent, const F32*, xBase*, U32)
{
    switch (toEvent) {
    case eEventVisible:
        RefreshBars();
        break;

    case eEventInvisible:
        HideBars();
        break;

    case eEventIncrement: {
        F32 oldVol = GetVolume(), vol = oldVol;
        vol += 1.0f / 11.0f;
        if (vol > 1.0f)
            vol = 1.0f;
        SetVolume(vol);
        ui->Signal(oldVol != vol ? eEventIncrementSuccess : eEventIncrementFailed);

        RefreshBars();
        break;
    }
    case eEventDecrement: {
        F32 oldVol = GetVolume(), vol = oldVol;
        vol -= 1.0f / 11.0f;
        if (vol < 0.0f)
            vol = 0.0f;
        SetVolume(vol);
        ui->Signal(oldVol != vol ? eEventDecrementSuccess : eEventDecrementFailed);

        RefreshBars();
        break;
    }
    }

    return true;
}

F32 zUICustomVolume::GetVolume() const
{
    return xSndMixerGetVolume(sndMixerFader);
}

void zUICustomVolume::SetVolume(F32 volume)
{
    xSndMixerSetVolume(sndMixerFader, volume);
}
