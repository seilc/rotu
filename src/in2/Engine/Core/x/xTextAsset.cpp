#include "xTextAsset.h"

#include "xstransvc.h"
#include "xString.h"

const char* xTextFindString(U32 key, U32* len)
{
    if (key == 0) {
        return NULL;
    }

    U32 size;
    const xTextAsset* asset = (const xTextAsset*)xSTFindAsset(key, &size);

    if (!asset) {
        return NULL;
    }

    if (len) {
        *len = asset->len;
    }

    return (const char*)((U8*)asset + sizeof(xTextAsset));
}

const char* xTextFindString(const char* key, U32* len)
{
    return xTextFindString(xStrHash(key), len);
}
