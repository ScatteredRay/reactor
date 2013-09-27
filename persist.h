// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _PERSIST_H_
#define _PERSIST_H_

#include "reflect.h"

template<typename T>
T* persist_from_config(const char* path)
{
    Reflect* reflect = get_reflection<T>();
    reflect->print();

    // TEMP
    return new T();
}

#endif //_PERSIST_H_
