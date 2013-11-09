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
};

REFLECT_TYPE(InternalObj)
{
    reflect(&InternalObj::J, "J");
    reflect(&InternalObj::K, "K");
    reflect(&InternalObj::L, "L");
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
    //reflect(&Environment::Layers, "Layers");
    reflect(&PersistObj::X, "X");
    reflect(&PersistObj::Y, "Y");
    //reflect(&Environment::Z, "Z");
    reflect(&PersistObj::W, "W");
    //reflect(&Environment::F, "F");
    //reflect(&Environment::G, "G");
    //reflect(&Environment::H, "H");
    reflect(&PersistObj::O, "O");
    reflect(&PersistObj::P, "P");
}

void TestPersist()
{
    PersistObj* p = persist_create_from_config<PersistObj>("persist.json");
    assert(p->X == 13);
    assert(p->Y == 18.0f);
    assert(p->W == true);

    assert(p->O.J == 7);
    assert(p->O.K == 24.0f);
    assert(p->O.L == -44);

    assert(p->P->J == 98);
    assert(p->P->K == 38.6f);
    assert(p->P->L == 4);
    delete p;
}


// So we don't need these in the project.
#define put_debug_string(x) // gross
#include <../source/reporting.cpp>
#include <../source/persist.cpp>
#include <../source/persist_json.cpp>
#include <../source/reflect.cpp>
