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
    U32 mNumSounds;
    xOneLiner* mSounds;

public:
    void InitWithSounds(xOneLiner* soundList, U32 maxCheck);
    void Reset();

    bool PlaySound(U32 r4)
    {
        if (r4 < mNumSounds) {
            return mSounds[r4].PlaySound();
        }
        return false;
    }

    iSndGroupHandle GetSoundGroup(U32 r4) const
    {
        if (r4 >= mNumSounds) {
            return ISND_INVALID_SND_GROUP_HANDLE;
        }
        return mSounds[r4].GetSoundGroup();
    }
};

#endif
