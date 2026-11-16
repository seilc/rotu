#ifndef ZLIGHTWEIGHT_H
#define ZLIGHTWEIGHT_H

#include "containers.h"
#include "xMemMgr.h"

class zLightweightSystemBase
{
public:
    virtual void SceneSetup() {}
    virtual void Reset() {}
    virtual void Update(F32) {}
    virtual void Render() {}
    virtual void PostRender2d() const {}
    virtual void PostRender3d() const {}
    virtual void Exit() {}
};

class zLightweightManager
{
public:
    static void RegisterLightweight(zLightweightSystemBase* newSystem);
};

template <class T, S32 N>
class zLightweight
{
    bool used;
    U32 id;

public:
    zLightweight(bool used)
    {
        this->used = used;
    }

    void SetUsed(bool value)
    {
        used = value;
    }

    virtual void Render() const {}
    virtual void PostRender2d() const {}
    virtual void PostRender3d() const {}
    virtual void Update(F32) {}
    virtual void Destroy() {}
    virtual void Reset() {}
};

template <class T>
class zLightweightList : public zLightweightSystemBase
{
    typedef typename pool_list<T>::node_type node_type;

public:
    void Init(S32 size)
    {
        zLightweightManager::RegisterLightweight(this);
        list.create(size, eMemMgrTag_Lightweight, NULL);
    }

    void Reset()
    {
        node_type* itr = list.begin();
        node_type* end = list.end();
        while (itr != end) {
            if (itr->value.IsActive()) {
                itr->value.Reset();
            }
            itr = itr->next;
        }
        list.clear();
    }

    void SceneSetup()
    {
        T::SceneSetup();
        node_type* currentNode = list.get_buffer();
        node_type* const endNode = currentNode + list.max_size();
        while (currentNode != endNode) {
            currentNode->value.Setup();
            currentNode++;
        }
    }

    void Update(F32 dt)
    {
        node_type* itr = list.begin();
        node_type* end = list.end();
        while (itr != end) {
            if (itr->value.IsActive()) {
                itr->value.Update(dt);
                itr = itr->next;
            } else {
                node_type* next = itr->next;
                list.erase(itr);
                itr = next;
            }
        }
    }

    void Render()
    {
        node_type* itr = list.begin();
        node_type* end = list.end();
        while (itr != end) {
            if (itr->value.IsActive()) {
                itr->value.Render();
            }
            itr = itr->next;
        }
    }

    void PostRender2d() const
    {
        node_type* itr = list.begin();
        node_type* end = list.end();
        while (itr != end) {
            if (itr->value.IsActive()) {
                itr->value.PostRender2d();
            }
            itr = itr->next;
        }
    }

    void PostRender3d() const
    {
        node_type* itr = list.begin();
        node_type* end = list.end();
        while (itr != end) {
            if (itr->value.IsActive()) {
                itr->value.PostRender3d();
            }
            itr = itr->next;
        }
    }

    void Exit()
    {
        node_type* currentNode = list.get_buffer();
        node_type* const endNode = currentNode + list.max_size();
        while (currentNode != endNode) {
            currentNode->value.Exit();
            currentNode++;
        }
    }

    T* GetFree()
    {
        if (list.size() >= list.max_size()) {
            return NULL;
        }
        list.push_front();
        return &list.front();
    }

protected:
    pool_list<T> list;
};

#endif
