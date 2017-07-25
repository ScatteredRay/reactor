// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "persist.h"
#include "memory.h"
#include <assert.h>

struct InternalObj
{
    int J;
    float K;
    int L;
    int M;

public:
    static int loaded_count;
    void persist_finish_loaded()
    {
        assert(M == 50);
        loaded_count++;
    }
};

int InternalObj::loaded_count;

REFLECT_TYPE(InternalObj)
{
    REFLECT_MEMBER(int, J, "J");
    REFLECT_MEMBER(float, K, "K");
    REFLECT_MEMBER(int, L, "L");
    REFLECT_MEMBER(int, M, "M");
}

struct PersistObj
{
    int X;
    float Y;
    int* Z;
    bool W;
    enum {my_enum} F;
    struct {} G;
    int H[3][4];
    InternalObj O;
    unique_ptr<InternalObj> P;
};

REFLECT_TYPE(PersistObj)
{
    REFLECT_MEMBER(int, X, "X");
    REFLECT_MEMBER(float, Y, "Y");
    //REFLECT_MEMBER(int*, Z, "Z");
    REFLECT_MEMBER(bool, W, "W");
    //REFLECT_MEMBER(enum, F, "F");
    //REFLECT_MEMBER(struct, G, "G");
    //REFLECT_MEMBER(int[3][4], G, "G");
    REFLECT_MEMBER(InternalObj, O, "O");
    REFLECT_MEMBER(unique_ptr<InternalObj>, P, "P");
}

void TestPersist()
{
    InternalObj::loaded_count = 0;
    PersistObj* p = persist_create_from_config<PersistObj>("persist.json");
    assert(p->X == 13);
    assert(p->Y == 18.0f);
    //assert(*(p->Z) == 98);
    assert(p->W == true);

    assert(p->O.J == 7);
    assert(p->O.K == 24.0f);
    assert(p->O.L == -44);
    assert(p->O.M == 50);

    assert(p->P->J == 98);
    assert(p->P->K == 38.6f);
    assert(p->P->L == 4);
    assert(p->P->M == 50);

    assert(InternalObj::loaded_count == 2);

    delete p;
}


// So we don't need these in the project.
#define put_debug_string(x) // gross
#include <../source/reporting.cpp>
#include <../source/persist.cpp>
#include <../source/persist_json.cpp>
#include <../source/reflect.cpp>
