// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _PERSIST_H_
#define _PERSIST_H_

template<typename T>
T* persist_create_from_config(const char* path);

void* persist_create_from_json_reflect(const char* path, class Reflect* reflect);

#include "persist.inl"

#endif //_PERSIST_H_
