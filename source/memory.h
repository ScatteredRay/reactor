// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
//
// "We have all forgot more than we remember" -- Thomas Fuller
//
#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <memory>
using std::unique_ptr;

// Clean this up if we ever get http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2013/n3601.html

template <typename T, void (*f)(T*)>
class delete_fun
{
public:
    
    void operator()(T* obj) const
    {
        f(obj);
    }
};

#endif //_MEMORY_H_
