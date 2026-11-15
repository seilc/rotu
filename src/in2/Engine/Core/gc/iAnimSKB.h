#ifndef IANIMSKB_H
#define IANIMSKB_H

#include "xMath3.h"

struct iAnimSKBHeader {
    U32 Magic;
    void* ExtractedData;
    U16 BoneCount;
    U16 TimeCount;
    U16 KeyCount;
    U16 TranCount;
    F32 Scale[3];
};

struct iAnimSKBKey {
    U16 Frame;
    U16 TranIndex;
    S16 Quat[3];
};

void iAnimEvalSKB(iAnimSKBHeader* data, F32 time, U32 flags, xVec3* tran, xQuat* quat);
F32 iAnimDurationSKB(iAnimSKBHeader* data);
void _iAnimSKBAdjustTranslate(iAnimSKBHeader* data, U32 bone, F32* starttran, F32* endtran);
S32 _iAnimSKBNumExtract(iAnimSKBHeader* data, U32 bone);
S32 _iAnimSKBExtractTranslateAxis(iAnimSKBHeader* data, U32 bone, xVec3* tranArray, S32 tranCount, U8* axis);
F32 QuatToYaw(xQuat* q);
void YawToQuat(F32 yaw, xQuat* q);
S32 _iAnimSKBExtractYaw(iAnimSKBHeader* data, U32 bone, F32* yawArray, S32 yawCount);
void _iAnimSKBAdjustYaw(iAnimSKBHeader* data, U32 bone, F32 yawStart, F32 yawEnd);

#endif
