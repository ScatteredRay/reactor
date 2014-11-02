// Copyright (c) 2014, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "types.h"
#include <assert.h>

void TestTypes()
{
    assert(sizeof(int8) == 1);
	assert(sizeof(uint8) == 1);
    assert(sizeof(int16) == 2);
	assert(sizeof(uint16) == 2);
    assert(sizeof(int32) == 4);
	assert(sizeof(uint32) == 4);
    assert(sizeof(int64) == 8);
	assert(sizeof(uint64) == 8);
}
