#include "containers.h"

void pool_list_base::push_front()
{
    node_base* n = alloc();
    n->next = head.next;
    n->prev = &head;
    n->next->prev = n; n->prev->next = n;
    _size++;
}

void pool_list_base::push_back()
{
    node_base* n = alloc();
    n->prev = tail.prev;
    n->next = &tail;
    n->next->prev = n; n->prev->next = n;
    _size++;
}

void pool_list_base::create(S32 node_size, S32 max_size, U32 memtag, void* buffer)
{
    if (!buffer)
        this->buffer = xMEMALLOC(node_size * max_size, 0);
    else
        this->buffer = buffer;

    _max_size = max_size;
    reset(node_size);
}

void pool_list_base::reset(S32 node_size)
{
    _size = 0;
    head.next = &tail;
    head.prev = 0;
    tail.prev = &head;
    tail.next = 0;
    stack = (node_base*)buffer;

    U8* mem = (U8*)buffer, *end_mem = mem + (_max_size-1) * node_size;
    for (; mem != end_mem; mem += node_size)
        ((node_base*)mem)->next = (node_base*)(mem + node_size);
    ((node_base*)mem)->next = 0;
}

pool_list_base::node_base* pool_list_base::insert(node_base* it)
{
    node_base* n = alloc();
    n->next = it;
    n->prev = it->prev;
    n->next->prev = n; n->prev->next = n;

    _size++;

    return n;
}

pool_list_base::node_base* pool_list_base::erase(node_base* it)
{
    node_base* next = it->next, *prev = it->prev;

    next->prev = prev;
    prev->next = next;

    free(it);

    _size--;

    return next;
}

pool_list_base::node_base* pool_list_base::erase(node_base* first, node_base* last)
{
    node_base* n = first, *end = last;

    n->prev->next = end;
    end->prev = n->prev;

    while (n != end) {
        node_base* next = n->next;
        free(n);
        n = next;
        _size--;
    }
    
    return last;
}
