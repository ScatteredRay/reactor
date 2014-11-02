// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include <assert.h>

void TestTypes();
void TestMemory();
void TestPersist();
void TestCollections();
void TestScopeDefer();

int main(int argc, char* argv)
{
	TestTypes();
    TestMemory();
    TestPersist();
    TestCollections();
    TestScopeDefer();
    return 0;
}
