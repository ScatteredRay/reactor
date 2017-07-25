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
    reflect(&InternalObj::J, "J");
    reflect(&InternalObj::K, "K");
    reflect(&InternalObj::L, "L");
    reflect(&InternalObj::M, "M");
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
    reflect(&PersistObj::X, "X");
    reflect(&PersistObj::Y, "Y");
    //reflect(&PersistObj::Z, "Z");
    reflect(&PersistObj::W, "W");
    //reflect(&PersistObj::F, "F");
    //reflect(&PersistObj::G, "G");
    //reflect(&PersistObj::H, "H");
    reflect(&PersistObj::O, "O");
    reflect(&PersistObj::P, "P");
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
