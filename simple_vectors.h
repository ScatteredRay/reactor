// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _SIMPLE_VECTORS_H_
#define _SIMPLE_VECTORS_H_

// These are just simple data structures, it isn't ment to replace a better
// more complete set of data structures.

// Try not to make any of the code actually depend on these, just use these
// as placeholders and for tests.

// Sony's Vector Math library is good:
// http://code.google.com/p/bullet/source/browse/#svn/trunk/Extras/vectormathlibrary

struct vector2
{
    float x;
    float y;
};

struct vector3
{
    float x;
    float y;
    float z;
};

struct vector4
{
    float x;
    float y;
    float z;
    float w;
};

#endif //_SIMPLE_VECTORS_H_
