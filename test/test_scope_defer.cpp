// Copyright (c) 2014, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "scope_defer.h"
#include <assert.h>

void TestScopeDefer()
{
    int count = 0;

    //TODO: check optimizer to verify that this gets optimized properlly.

    assert(++count == 1);
    SCOPE_DEFER(assert(++count == 8));
    assert(++count == 2);
    SCOPE_DEFER(assert(++count == 7));
    assert(++count == 3);
    SCOPE_DEFER(assert(++count == 6));
    assert(++count == 4);
    SCOPE_DEFER(assert(++count == 5));
}
