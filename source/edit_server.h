// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _EDIT_SERVER_H_
#define _EDIT_SERVER_H_

#define WITH_EDIT_SERVER

#ifdef WITH_EDIT_SERVER

struct EditServer;

EditServer* InitEditServer();
void DestroyEditServer(EditServer* srv);
void UpdateEditServer(EditServer* srv);

#endif //WITH_EDIT_SERVER

#endif //_EDIT_SERVER_H_
