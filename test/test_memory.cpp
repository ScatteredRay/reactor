// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "memory.h"
#include <assert.h>

int allocation_count;
int deallocation_count;

class MemoryTestObj
{
public:
    MemoryTestObj()
    {
        allocation_count++;
    }

    ~MemoryTestObj()
    {
        deallocation_count++;
    }
};

int free_count;

void testFree(MemoryTestObj* obj)
{
    free_count++;
    delete obj;
}

void TestMemory()
{
    allocation_count = 0;
    deallocation_count = 0;
    free_count = 0;

    {
        unique_ptr<MemoryTestObj> obj1 = unique_ptr<MemoryTestObj>(new MemoryTestObj());
        unique_ptr<MemoryTestObj> obj2(new MemoryTestObj());
        unique_ptr<MemoryTestObj, delete_fun<MemoryTestObj, testFree>> obj3(new MemoryTestObj());
        unique_ptr<MemoryTestObj, void (*)(MemoryTestObj*)> obj4(new MemoryTestObj(), testFree);

        assert(sizeof(obj1) == sizeof(void*));
        assert(sizeof(obj2) == sizeof(void*));
        assert(sizeof(obj3) == sizeof(void*));
        assert(sizeof(obj4) == sizeof(void*) * 2);

        assert(allocation_count == 4);

        unique_ptr<MemoryTestObj[]> objAr(new MemoryTestObj[4]);
        assert(sizeof(objAr) == sizeof(void*));

        assert(free_count == 0);
        assert(deallocation_count == 0);
        assert(allocation_count == 8);
    }
    
    assert(free_count == 2);
    assert(allocation_count == 8);
    assert(allocation_count == deallocation_count);
}
