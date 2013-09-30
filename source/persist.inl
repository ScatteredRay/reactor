// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "reflect.h"
#include "reporting.h"

template<typename T>
T* persist_create_from_config(const char* path)
{
    Reflect* reflect = get_reflection<T*>();
    reflect->heap_alloc();
    reflect->print();

    return (T*)persist_create_from_json_reflect(path, reflect);

}
