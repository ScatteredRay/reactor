// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#ifdef _WIN32
#include "sys_win.h"
#endif // WIN32

// Converts Ascii to the system keycodes.
int GetKeyCodeFromAscii(char key);

#endif //_SYSTEM_H_
