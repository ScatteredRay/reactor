// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _SYS_WIN_H_
#define _SYS_WIN_H_

const unsigned int MAX_KEYCODES = 255;

#if _MSC_VER < 1900
#define snprintf _snprintf
#endif

#endif// _SYS_WIN_H_
