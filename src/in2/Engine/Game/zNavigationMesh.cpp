#include "xCommon.h"
#include "zNavigationMesh.h"

#include "xNavigationMeshAsset.h"
#include "zNPCMgr.h"
#include "zPlayer.h"

zNavigationMesh* zNavigationMesh::activeNavMesh;

void zNavigationMesh_Init(void* base, void* asset)
{
    ((zNavigationMesh*)base)->Init((xNavigationMeshAsset*)asset);
}

void zNavigationMeshEventCB(xBase*, xBase* to, U32, const F32*, xBase*, U32)
{
    zNavigationMesh* navMesh = (zNavigationMesh*)to;
}

void zNavigationMesh::Init(xNavigationMeshAsset* asset)
{
    xBaseInit(this, asset);
    mAsset = asset;
    AddNavMeshToLinkedList(this);
    FindActiveNavMesh();
    eventFunc = zNavigationMeshEventCB;
}

void zNavigationMesh::SceneReset()
{
    zNavigationMesh* activeNavMesh = zNavigationMesh::activeNavMesh;

    zNavigationMesh* curNavMesh = activeNavMesh;
    do {
        xBaseInit(curNavMesh, curNavMesh->mAsset);
        curNavMesh = curNavMesh->nextNavMesh;
    } while (curNavMesh != activeNavMesh);

    activeNavMesh->FindActiveNavMesh();
}

void zNavigationMesh::SceneEnd()
{
    activeNavMesh = NULL;
}

zMeshCircle* zNavigationMesh::FindMeshCircleForEnt(const xEnt* ent)
{
    zNavigationMesh* currMesh = activeNavMesh;
    zMeshCircle* circle = NULL;

    do {
        circle = currMesh->GetNavMeshAsset()->FindMeshCircleForEnt(ent);
        if (circle)
            break;
        currMesh = currMesh->nextNavMesh;
    } while (currMesh != activeNavMesh);

    return circle;
}

zNavigationMesh* zNavigationMesh::FindMeshForPosition(const xVec3& pos, S32& submeshIdx, S32& triIdx)
{
    zNavigationMesh* currMesh = GetActiveNavMesh();

    do {
        if (currMesh->GetNavMeshAsset()->GetTriangle(pos, 0, 0, submeshIdx, triIdx))
            return currMesh;
        currMesh = currMesh->nextNavMesh;
    } while (currMesh != GetActiveNavMesh());

    return NULL;
}

void zNavigationMesh::AddNavMeshToLinkedList(zNavigationMesh* navMesh)
{
    if (activeNavMesh == NULL) {
        activeNavMesh = navMesh;
        navMesh->nextNavMesh = navMesh;
    } else {
        navMesh->nextNavMesh = activeNavMesh->nextNavMesh;
        activeNavMesh->nextNavMesh = navMesh;
    }
}

void zNavigationMesh::FindActiveNavMesh()
{
    zNavigationMesh* testNavMesh = activeNavMesh;

    do {
        if (testNavMesh->IsEnabled()) {
            activeNavMesh = testNavMesh;
            break;
        }
        testNavMesh = testNavMesh->nextNavMesh;
    } while (testNavMesh != activeNavMesh);
}

void zNavigationMesh::SetEnable(bool state)
{
    if (state)
        baseFlags |= k_XBASE_IS_ENABLED;
    else
        baseFlags &= (U16)~k_XBASE_IS_ENABLED;
}

bool zNavigationMesh::IsEnabled()
{
    return baseFlags & k_XBASE_IS_ENABLED;
}

void zNavigationMesh::SetNavigationMeshActive()
{
    zNavigationMesh* testNavMesh = activeNavMesh;
    do {
        if (testNavMesh->IsEnabled())
            testNavMesh->SetEnable(false);
        testNavMesh = testNavMesh->nextNavMesh;
    } while (testNavMesh != activeNavMesh);

    SetEnable(true);
    activeNavMesh = this;

    zPlayer::SwitchedNavmeshes();
    zNPCMgr_switchedNavmeshes();
}

zNavigationMesh* zNavigationMesh::GetActiveNavMesh()
{
    return activeNavMesh;
}

xNavigationMeshAsset* zNavigationMesh::GetActiveNavMeshAsset()
{
    zNavigationMesh* activeNavMesh = GetActiveNavMesh();
    if (!activeNavMesh)
        return NULL;
    return activeNavMesh->GetNavMeshAsset();
}
