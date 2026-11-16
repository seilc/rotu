#ifndef ZFXSCREENCRACK_H
#define ZFXSCREENCRACK_H

#include "types.h"

struct xVec2;
class zFXScreenCrackSegment;

class zFXScreenCrack
{
public:
    static void Init();
    bool Start(const xVec2& pos, const xVec2& dir, F32 life);
    void Grow();
    const xVec2& GetPosition();

private:
    zFXScreenCrackSegment* root;
};

#endif
