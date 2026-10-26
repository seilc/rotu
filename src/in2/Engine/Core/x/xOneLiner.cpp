#include "xOneLiner.h"

#include "xDebug.h"
#include "iTime.h"
#include "zUIIncredibles.h"
#include "zConversation.h"
#include "xRand.h"
#include "xSndMgr.h"
#include "xString.h"

xOneLiner* xOneLiner::sDelayedSound;

#if DEBUG || RELEASE
bool xOneLiner::sDebugIgnoreTime;
bool xOneLiner::sDebugIgnoreProb;
#endif

void xOneLiner::SceneInit()
{
    SceneReset();
}

void xOneLiner::SceneReset()
{
    sDelayedSound = NULL;
}

void xOneLiner::UpdateSounds()
{
    if (sDelayedSound && iTimeGetGame() >= sDelayedSound->mSoonestTimeToPlay) {
        xOneLiner* sound = sDelayedSound;
        sDelayedSound = NULL;
        sound->PlaySoundCore();
    }
}

inline bool xOneLiner::NoInhibitions() const
{
    return (HasValidHandle()
         && !(mFlags & 0x4000)
         && !xSndMgrIsDialogPlaying()
         && !zConversationIsConversing());
}

inline bool xOneLiner::IsProbable() const
{
    U32 probBits = mFlags & 0x3F80;
    if (probBits == 0) return true;

    F32 prob = 0.01f * (probBits >> 7);
    return (prob > xrand_RandomFloat());
}

bool xOneLiner::PlaySoundCore()
{
    if (NoInhibitions()) {
        F32 timeNow = iTimeGetGame();
        
        F32 timeToPlay;
        if (mFlags & 0x40) {
            timeToPlay = mSoonestTimeToPlay;
        } else {
            timeToPlay = mSoonestTimeToPlay + 45.0f * zUIIncrediblesGetGameProgress();
        }

        if (timeNow >= timeToPlay) {
            if (!sDelayedSound && IsProbable()) {
                zConversationPlay(mSoundGroupHdl, NULL);
                mSoonestTimeToPlay = timeNow + CalcTimeToWait();
                if (mFlags & 0x10) {
                    mFlags |= 0x4000;
                }
                return true;
            }
        } else if (mFlags & 0x20) {
            mSoonestTimeToPlay = timeNow + CalcTimeToWait();
        }
    }
    return false;
}

bool xOneLiner::PlaySound()
{
    S32 delayBits = mFlags & 0xF;
    if (delayBits) {
        return PlaySoundDelayed(0.1f * delayBits);
    }
    return PlaySoundCore();
}

bool xOneLiner::PlaySoundDelayed(F32 delay)
{
    if (!sDelayedSound) {
        F32 timeNow = iTimeGetGame();
        F32 futureTime = timeNow + delay;
        if (futureTime >= mSoonestTimeToPlay) {
            mSoonestTimeToPlay = futureTime;
            sDelayedSound = this;
            return true;
        }
    }
    return false;
}

F32 xOneLiner::CalcTimeToWait() const
{
    return mCycleTime;
}

void xOneLinerList::InitWithSounds(xOneLiner* soundList, U32 maxCheck)
{
    F32 timeNow = iTimeGetGame();
    mNumSounds = 0;
    mSounds = soundList;
    while (soundList->mAssetName) {
        soundList->Init();
        mNumSounds++;
        soundList++;
    }
    xASSERT(maxCheck == 0 || maxCheck == mNumSounds);
}

inline void xOneLiner::Init()
{
    mSoonestTimeToPlay = iTimeGetGame() + xrand_RandomFloatRange(0.2f, 0.5f) * CalcTimeToWait();
    mSoundGroupHdl = mAssetName[0] ? xSndMgrGetSoundGroup(xStrHash(mAssetName)) : ISND_INVALID_SND_GROUP_HANDLE;
}

void xOneLinerList::Reset()
{
    xOneLiner* sp = mSounds;
    for (U32 i = 0; i < mNumSounds; i++, sp++) {
        sp->Reset();
    }
}

inline void xOneLiner::Reset()
{
    mFlags &= ~0x4000;
}
