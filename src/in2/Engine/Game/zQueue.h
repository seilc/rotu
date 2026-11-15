#ifndef ZQUEUE_H
#define ZQUEUE_H

#include "types.h"

template <class T, S32 max_size> class zQueue
{
public:
    S32 next_offset(S32 offset) { return (first + offset) % max_size; }

    S32 get_size_queue() const { return size; }

    void clear_queue()
    {
        first = size = 0;
    }

    T* fetch_queue_ptr_offset(S32 offs)
    {
        if (size <= 0) {
            return NULL;
        }

        return &data[next_offset(offs)];
    }

    void fetch_queue_copy_offset(T& elem, S32 offs)
    {
        if (size <= 0) {
            return;
        }

        elem = data[next_offset(offs)];
    }

    void get_queue(T& elem, S32 offs)
    {
        if (size <= 0) {
            return;
        }

        if (size == 1) {
            get_queue(elem);
            return;
        }

        S32 index = next_offset(offs);
        S32 last = next_offset(size - 1);
        swap(index, last);
        get_queue_last(elem);
    }

    void get_queue_last(T& elem)
    {
        if (size <= 0) {
            return;
        }

        elem = data[next_offset(size - 1)];
        size--;
    }

    void swap(S32 a, S32 b)
    {
        T tmp = data[a];
        data[a] = data[b];
        data[b] = tmp;
    }

    void get_queue(T& elem)
    {
        if (size <= 0) {
            return;
        }

        elem = data[first];
        first = next_offset(1);
        size--;
    }

    S32 add_queue(T& elem)
    {
        S32 index = next_offset(size);
        data[index] = elem;
        next();
        return index;
    }

    void next()
    {
        if (size >= max_size) {
            first = next_offset(1);
        } else {
            size++;
        }
    }

protected:
    S32 first;
    S32 size;
    T data[max_size];
};

#endif
