// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _EDIT_SERVER_H_
#define _EDIT_SERVER_H_

#define WITH_EDIT_SERVER

#ifdef WITH_EDIT_SERVER

#include "reflect.h"

struct EditServer;

EditServer* InitEditServer();
void DestroyEditServer(EditServer* srv);
void UpdateEditServer(EditServer* srv);
void EditServerAddObject(EditServer* srv, const char* name, void* ptr, Reflect* reflect);

template <typename T>
void EditServerAddObject(EditServer* srv, const char* name, T* ptr)
{
    EditServerAddObject(srv, name, ptr, get_reflection<T>());
}

#endif //WITH_EDIT_SERVER

#endif //_EDIT_SERVER_H_
