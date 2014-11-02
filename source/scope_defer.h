// Copyright (c) 2014, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _SCOPE_DEFER_H_
#define _SCOPE_DEFER_H_

template<typename fn>
struct ScopeDefer
{
    fn defer;
    ScopeDefer(fn df) : defer(df) {}
    ~ScopeDefer()
    {
        defer();
    }
};

//#define SCOPE_DEFER(stmt) ScopeDefer scope_defer_var ## __FILE__ ## __COUNTER__ [&](){stmt}
#define SCOPE_DEFER(stmt)// ScopeDefer scope_defer_var ## __FILE__ ## __COUNTER__ [&](){stmt}

#endif //_SCOPE_DEFER_H_
