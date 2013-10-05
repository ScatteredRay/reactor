// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _COLLECTIONS_H_
#define _COLLECTIONS_H_

#include "stdlib.h"
#include <assert.h>

template<typename t>
class StaticArray
{
    unsigned int length;
    t* data;
public:
    StaticArray() : length(0), data(NULL) {}

    StaticArray(unsigned int len) : length(len)
    {
        data = new t[length];
    }

    ~StaticArray()
    {
        clear();
    }

    void reinit(unsigned int len)
    {
        length = len;
        delete[] data;
        data = new t[length];
    }

    void clear()
    {
        length = 0;
        delete[] data;
        data = NULL;
    }

    // We should consider if we want a resize, which would keep existing
    // contents intact, It would then not be save to hold an internal pointer.

    t& operator[](unsigned int index)
    {
        assert(index < length);
        return data[index];
    }

    const t& operator[](unsigned int index) const
    {
        (*this)[index];
    }

    unsigned int size() const
    {
        return length;
    }
};

/*template <typename T>
void Reflect_Type<StaticArray<T>>(Reflect& reflect)
{
    reflect(&length, "Length").unpersist(
        [] (void* ar, void* len) {
            ((StaticArray*)ar)->reinit(*(unsigned int*)len);
        });

    /*reflect(&data, "Contents").array().unpersist(
        [] (void* ar, T val, unsigned int index) {
            *((StaticArray*)ar)[index] = val;
            });*/
//}*/

#endif //_COLLECTIONS_H_
