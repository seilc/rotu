#include "xCommon.h"
#include "zFXScreenCrack.h"

#include "iMath.h"
#include "xGlobals.h"
#include "xMath2.h"
#include "xMemMgr.h"
#include "xRand.h"
#include "xstransvc.h"
#include "zHashes.h"
#include "zLightWeight.h"
#include "zRenderState.h"

#define MAX_SEGMENTS 64

class zFXScreenCrackSegment
{
public:
    bool Start(const xVec2& pos, const xVec2& dir, F32 minLength, F32 maxLength, F32 life, bool branches);
    void StartWithDir(const xVec2& pos, const xVec2& dir, F32 length, F32 life, bool branches);
    bool CreateBranch(zFXScreenCrackSegment& parent);
    void Grow();

    F32 GetLength() const { return length; }
    const xVec2& GetDir() const { return dir; }
    const xVec2& GetStart() const { return start; }
    bool IsActive() const { return active; }

    static void SceneSetup() {}
    void Setup() {}
    void Reset() {}
    void Update(F32 dt);
    void Render() {}
    void PostRender2d() const;
    void PostRender3d() const {}
    void Exit() {}

private:
    void SpawnBranch(F32 life)
    {
        this->life = life;
        spawned = true;
    }

    void AddBranch(zFXScreenCrackSegment* branch)
    {
        branch->nextSibling = firstBranch;
        firstBranch = branch;
    }

    F32 growSpeed;
    F32 progress;
    F32 length;
    F32 life;
    F32 branchPos;
    xVec2 start;
    xVec2 dir;
    bool active;
    bool spawned;
    U8 growCount;
    zFXScreenCrackSegment* firstBranch;
    zFXScreenCrackSegment* nextSibling;
};

namespace {

void Rotate2d(xVec2& out, const xVec2& v, F32 angle)
{
    F32 c = icos(angle), s = isin(angle);
    out.Scale(v, c);
    out.x += -s * v.y;
    out.y += s * v.x;
}

void Rotate2d(xVec2& v, F32 angle)
{
    xVec2 temp = v;
    Rotate2d(v, temp, angle);
}

inline void SetVertex(RwIm2DVertex& vert, const xVec2& pos, F32 u, F32 v, U8 alpha)
{
    RwIm2DVertexSetScreenX(&vert, pos.x * FB_XRES);
    RwIm2DVertexSetScreenY(&vert, pos.y * FB_YRES);
    RwIm2DVertexSetIntRGBA(&vert, 255, 255, 255, alpha);
    RwIm2DVertexSetU(&vert, u, 1.0f);
    RwIm2DVertexSetV(&vert, v, 1.0f);
}

typedef zLightweightList<zFXScreenCrackSegment> SegmentListBase;

class SegmentList : public SegmentListBase
{
public:
    virtual void Init(S32 size)
    {
        RwTexture* texture = (RwTexture*)xSTFindAsset(HASH_fx_nuke_crack, NULL);
        if (texture) {
            raster = RwTextureGetRaster(texture);
        } else {
            raster = NULL;
        }
        SegmentListBase::Init(size);
    }

    F32 Intersection(const xVec2& pos, const xVec2& dir, F32 maxDist, const zFXScreenCrackSegment* ignore)
    {
        node_type* itr = list.begin();
        node_type* end = list.end();
        while (itr != end) {
            zFXScreenCrackSegment* seg = &itr->value;
            if (seg->IsActive() && seg != ignore) {
                F32 slope = dir.x / dir.y,
                    t = (pos.x + (slope * seg->GetStart().y - slope * pos.y) - seg->GetStart().x) /
                        (seg->GetDir().x - slope * seg->GetDir().y),
                    dist = (seg->GetStart().y + t * seg->GetDir().y - pos.y) / dir.y;
                if (t < seg->GetLength() && t >= 0.0f && dist < maxDist && dist > EPSILON) {
                    maxDist = dist;
                }
            }
            itr = itr->next;
        }
        return maxDist;
    }

    RwIm2DVertex* GetVertices(S32 count)
    {
        S32 index = numVertices;
        numVertices += count;
        return &vertices[index];
    }

    void PostRender2d() const
    {
        zRenderState(eRenderStateScrFX);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, raster);
        if (list.size() > 0) {
            S32 count = list.size() * 6;
            vertices = (RwIm2DVertex*)xMEMPUSHTEMP(count * sizeof(RwIm2DVertex));
            RwReal nearZ = RwIm2DGetNearScreenZ();
            for (S32 i = 0; i < count; i++) {
                RwIm2DVertexSetScreenZ(&vertices[i], nearZ);
            }
            numVertices = 0;
            SegmentListBase::PostRender2d();
            if (numVertices > 0) {
                RwIm2DRenderPrimitive(rwPRIMTYPETRILIST, vertices, numVertices);
            }
            xMEMPOPTEMP(vertices);
        }
    }

private:
    RwRaster* raster;
    mutable RwIm2DVertex* vertices;
    mutable S32 numVertices;
};

SegmentList segments;

} // namespace

bool zFXScreenCrackSegment::Start(const xVec2& pos, const xVec2& dir, F32 minLength, F32 maxLength, F32 life, bool branches)
{
    for (S32 i = 0; i < 5; i++) {
        xVec2 newDir;
        Rotate2d(newDir, dir, xrand_RandomFloatRange((PI / 180.0f) * -30.0f, (PI / 180.0f) * 30.0f));
        F32 dist = segments.Intersection(pos, newDir, maxLength, NULL);
        if (dist >= minLength) {
            StartWithDir(pos, newDir, xrand_RandomFloatRange(minLength, dist), life, branches);
            return true;
        }
    }
    active = false;
    spawned = false;
    return false;
}

void zFXScreenCrackSegment::PostRender2d() const
{
    if (!spawned) {
        return;
    }

    xVec2 side = { -dir.y, dir.x };
    U8 alpha = 127.5f * MIN(1.0f, life);
    xVec2 end;
    end.AddScale(start, dir, progress * length);

    RwIm2DVertex* verts;
    if (growCount) {
        verts = segments.GetVertices(6);
    } else {
        verts = segments.GetVertices(3);
    }

    xVec2 pos;
    pos.AddScale(start, side, -0.004f);
    SetVertex(verts[0], pos, 0.0f, 0.0f, alpha);
    pos.AddScale(start, side, 0.004f);
    SetVertex(verts[1], pos, 0.0f, 1.0f, alpha);
    if (growCount) {
        pos.AddScale(end, side, -0.004f);
        SetVertex(verts[2], pos, 1.0f, 0.0f, alpha);
        verts[3] = verts[2];
        verts[4] = verts[1];
        pos.AddScale(end, side, 0.004f);
        SetVertex(verts[5], pos, 1.0f, 1.0f, alpha);
    } else {
        SetVertex(verts[2], end, 1.0f, 0.5f, alpha);
    }
}

void zFXScreenCrackSegment::Update(F32 dt)
{
    if (!spawned) {
        return;
    }

    life -= dt;
    if (life <= 0.0f) {
        active = false;
    } else if (progress < 1.0f) {
        F32 newProgress = progress + growSpeed * dt;
        for (zFXScreenCrackSegment* branch = firstBranch; branch != NULL; branch = branch->nextSibling) {
            if (branch->branchPos < newProgress && branch->branchPos >= progress) {
                branch->SpawnBranch(life);
            }
        }
        progress = newProgress;
        if (progress > 1.0f) {
            progress = 1.0f;
        }
    }
}

void zFXScreenCrackSegment::Grow()
{
    if (segments.IsFull()) {
        return;
    }

    for (zFXScreenCrackSegment* branch = firstBranch; branch != NULL; branch = branch->nextSibling) {
        branch->Grow();
    }

    if (segments.IsFull()) {
        return;
    }

    growCount++;
    if (growCount > 1) {
        return;
    }

    xVec2 end;
    end.AddScale(start, dir, length);
    F32 minLength = 0.4f * length, maxLength = 0.8f * length;

    if (segments.FreeCount() >= 2 && xrand_GenRandFloat() < 0.15f) {
        F32 angle = xrand_RandomFloatRange((PI / 180.0f) * 30.0f, (PI / 180.0f) * 60.0f);
        xVec2 newDir;
        S32 i;
        for (i = 0; i < 5; i++) {
            Rotate2d(newDir, dir, xrand_RandomFloatRange((PI / 180.0f) * -60.0f, 0.0f));
            F32 dist = segments.Intersection(end, newDir, maxLength, NULL);
            if (dist >= minLength) {
                zFXScreenCrackSegment* branch = segments.GetFree();
                branch->StartWithDir(end, newDir, xrand_RandomFloatRange(minLength, dist), life, false);
                AddBranch(branch);
                break;
            }
        }
        if (i < 5) {
            for (i = 0; i < 5; i++) {
                Rotate2d(newDir, angle);
                F32 dist = segments.Intersection(end, newDir, maxLength, NULL);
                if (dist >= minLength) {
                    zFXScreenCrackSegment* branch = segments.GetFree();
                    branch->StartWithDir(end, newDir, xrand_RandomFloatRange(minLength, dist), life, false);
                    AddBranch(branch);
                    break;
                }
            }
        }
    } else {
        zFXScreenCrackSegment* branch = segments.GetFree();
        if (branch->Start(end, dir, minLength, maxLength, life, false)) {
            AddBranch(branch);
        }
    }
}

void zFXScreenCrackSegment::StartWithDir(const xVec2& pos, const xVec2& dir, F32 length, F32 life, bool branches)
{
    this->life = life;
    nextSibling = NULL;
    firstBranch = NULL;
    active = true;
    spawned = true;
    start = pos;
    this->dir = dir;
    this->length = length;
    growCount = 0;
    progress = 0.0f;
    growSpeed = xrand_RandomFloatRange(5.0f, 20.0f);
    if (branches) {
        for (S32 i = 0; i < 2 && !segments.IsFull(); i++) {
            if (xrand_GenRandFloat() > 0.5f) {
                continue;
            }
            segments.GetFree()->CreateBranch(*this);
        }
    }
}

bool zFXScreenCrackSegment::CreateBranch(zFXScreenCrackSegment& parent)
{
    F32 minLength = 0.4f * parent.length, maxLength = 0.8f * parent.length;
    for (S32 i = 0; i < 5; i++) {
        branchPos = xrand_RandomFloatRange(0.1f, 0.9f);
        start.AddScale(parent.start, parent.dir, branchPos * parent.length);
        F32 angle = xrand_RandomFloatRange((PI / 180.0f) * 20.0f, (PI / 180.0f) * 60.0f);
        // BUG: xrand_RandomChoice(1) is always 0
        if (xrand_RandomChoice(1)) {
            angle = -angle;
        }
        Rotate2d(dir, parent.dir, angle);
        F32 dist = segments.Intersection(start, dir, maxLength, NULL);
        if (dist >= minLength) {
            active = true;
            spawned = false;
            growCount = 0;
            firstBranch = NULL;
            parent.AddBranch(this);
            length = xrand_RandomFloatRange(minLength, dist);
            progress = 0.0f;
            growSpeed = xrand_RandomFloatRange(5.0f, 20.0f);
            return true;
        }
    }
    active = false;
    spawned = false;
    return false;
}

void zFXScreenCrack::Init()
{
    segments.Init(MAX_SEGMENTS);
}

bool zFXScreenCrack::Start(const xVec2& pos, const xVec2& dir, F32 life)
{
    root = segments.GetFree();
    if (root) {
        return root->Start(pos, dir, 0.11f, 0.2f, life, true);
    }
    return false;
}

void zFXScreenCrack::Grow()
{
    if (root) {
        root->Grow();
    }
}

const xVec2& zFXScreenCrack::GetPosition()
{
    return root->GetStart();
}
