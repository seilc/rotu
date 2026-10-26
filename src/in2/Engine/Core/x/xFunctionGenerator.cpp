#include "xFunctionGenerator.h"

#include "xScene.h"
#include "xGlobals.h"
#include "zEvent.h"

namespace {

void FunctionGeneratorEventWrapper(xBase* from, xBase* to, U32 toEvent, const F32* toParam, xBase* toParamWidget, U32 toParamWidgetID)
{
    ((xFunctionGenerator*)to)->HandleEvent(from, toEvent, toParam, toParamWidget, toParamWidgetID);
}

}

xFunctionGenerator::xFunctionGenerator(const xFunctionGeneratorAsset* asset)
{
    this->asset = asset;

    xBaseInit(this, asset);

    eventFunc = FunctionGeneratorEventWrapper;
    link = (xLinkAsset*)((U8*)asset + sizeof(xFunctionGeneratorAsset));
    started = false;
    currentState = false;
}

void xFunctionGenerator::Update(F32 dt)
{
    if (!IsEnabled()) return;

    currentTime += dt;

    if (!override) {
        if (asset->middleEnabled) {
            if (currentTime < asset->middleTime) {
                F32 t = currentTime / asset->middleTime;
                currentCycleWidth = asset->startCycleWidth + t * (asset->middleCycleWidth - asset->startCycleWidth);
                currentPulseWidth = asset->startPulseWidth + t * (asset->middlePulseWidth - asset->startPulseWidth);
            } else {
                currentCycleWidth = asset->middleCycleWidth;
                currentPulseWidth = asset->middlePulseWidth;
            }
        }
        if (asset->endEnabled) {
            if (currentTime < asset->endTime) {
                if (!asset->middleEnabled) {
                    F32 t = currentTime / asset->endTime;
                    currentCycleWidth = asset->startCycleWidth + t * (asset->endCycleWidth - asset->startCycleWidth);
                    currentPulseWidth = asset->startPulseWidth + t * (asset->endPulseWidth - asset->startPulseWidth);
                } else if (currentTime > asset->middleTime) {
                    F32 t = (currentTime - asset->middleTime) / (asset->endTime - asset->middleTime);
                    currentCycleWidth = asset->middleCycleWidth + t * (asset->endCycleWidth - asset->middleCycleWidth);
                    currentPulseWidth = asset->middlePulseWidth + t * (asset->endPulseWidth - asset->middlePulseWidth);
                }
            } else {
                currentCycleWidth = asset->endCycleWidth;
                currentPulseWidth = asset->endPulseWidth;
            }
        }
    }

    bool firedAny;
    do {
        firedAny = false;

        if (!currentState) {
            F32 currentOffWidth = currentCycleWidth - currentPulseWidth;
            xASSERT_L(122, currentOffWidth > 0.0f);

            if (currentTime - lastDownTime > currentOffWidth) {
                currentState = true;
                firedAny = true;
                zEntEvent(this, this, eEventTrue);
                if (!IsEnabled()) return;
                lastUpTime = lastDownTime + currentOffWidth;
            }
        }

        if (currentState) {
            if (currentTime - lastUpTime > currentPulseWidth) {
                currentState = false;
                firedAny = true;
                zEntEvent(this, this, eEventFalse);
                if (!IsEnabled()) return;
                lastDownTime = lastUpTime + currentPulseWidth;
            }
        }
    } while (firedAny);

    if (asset->endEnabled && currentTime >= asset->endTime) {
        zEntEvent(this, this, eEventExpired);
        started = false;
    }
}

void xFunctionGenerator::HandleEvent(xBase*, U32 toEvent, const F32* toParam, xBase*, U32)
{
    switch (toEvent) {
    case eEventEnable:
        xBaseEnable(this);
        break;
    case eEventDisable:
        xBaseDisable(this);
        break;
    case eEventReset:
    case eEventDebugReset:
        Reset();
        break;
    case eEventStart:
        Start();
        break;
    case eEventStop:
        Stop();
        break;
    case eEventOverrideFreqency:
        override = true;
        currentCycleWidth = toParam[0];
        currentPulseWidth = toParam[1];
        break;
    case eEventResetFrequency:
        override = false;
        break;
    }
}

void xFunctionGenerator::Start()
{
    currentCycleWidth = asset->startCycleWidth;
    currentPulseWidth = asset->startPulseWidth;
    currentTime = 0.0f;
    lastUpTime = 0.0f;
    lastDownTime = 0.0f;
    currentState = false;
    started = true;
    override = false;
}

void xFunctionGenerator::Stop()
{
    started = false;
}

void xFunctionGenerator::Reset()
{
    started = false;
}

void xFunctionGenerator_Init(xBase& data, xDynAsset& asset, size_t)
{
    new (&data) xFunctionGenerator((const xFunctionGeneratorAsset*)&asset);
}
