// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "persist.h"
#include <assert.h>

namespace Test
{

    struct InternalObj
    {
        int J;
        float K;
        int L;
    };

    template <>
    struct ::Reflect_Type<InternalObj>
    {
        static void metadata(Reflect& reflect)
        {
            reflect(&InternalObj::J, "J");
            reflect(&InternalObj::K, "K");
            reflect(&InternalObj::L, "L");
        }
    };

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
    };

    template <>
    struct ::Reflect_Type<PersistObj>
    {
        static void metadata(Reflect& reflect)
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
        }
    };


    void TestPersist()
    {
        PersistObj* p = persist_create_from_config<PersistObj>("persist.json");
        assert(p->X == 13);
        assert(p->Y == 18.0f);
        assert(p->W == true);

        assert(p->O.J == 7);
        assert(p->O.K == 24.0f);
        assert(p->O.L == -44);
        delete p;
    }

}

// So we don't need these in the project.
#define put_debug_string(x) // gross
#include <../source/reporting.cpp>
#include <../source/persist.cpp>
#include <../source/persist_json.cpp>
#include <../source/reflect.cpp>