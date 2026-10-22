#ifndef XONELINER_H
#define XONELINER_H

#include "iSnd.h"

class xOneLiner
{
public:
    char* mAssetName;
    F32 mCycleTime;
    U32 mFlags;
    F32 mSoonestTimeToPlay;
    iSndGroupHandle mSoundGroupHdl;

    static xOneLiner* sDelayedSound;
    static bool sDebugIgnoreTime;
    static bool sDebugIgnoreProb;

    static void SceneInit();
    static void SceneReset();
    static void UpdateSounds();
    static void TweaksInit();

    bool NoInhibitions() const;
    bool IsProbable() const;
    F32 CalcTimeToWait() const;
    bool PlaySoundCore();
    bool PlaySound();
    bool PlaySoundDelayed(F32 delay);
    void Init();
    void Reset();

    bool HasValidHandle() const
    {
        return (mSoundGroupHdl != ISND_INVALID_SND_GROUP_HANDLE);
    }

    iSndGroupHandle GetSoundGroup() const
    {
        return mSoundGroupHdl;
    }
};

class xOneLinerList
{
public:
    U32 mNumSounds;
    xOneLiner* mSounds;

    void InitWithSounds(xOneLiner* soundList, U32 maxCheck);
    void Reset();

    bool PlaySound(U32 index)
    {
        if (index < mNumSounds) {
            return mSounds[index].PlaySound();
        }
        return false;
    }

    iSndGroupHandle GetSoundGroup(U32 index) const
    {
        if (index >= mNumSounds) {
            return ISND_INVALID_SND_GROUP_HANDLE;
        }
        return mSounds[index].GetSoundGroup();
    }
};

#endif
