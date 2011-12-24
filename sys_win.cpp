// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include <assert.h>

#include "view_decl.h"
#include "gl_all.h"

int InitWindowAndLoop(int argc, char** argv)
{
    void InitGLExt();
    InitGLExt();

    ViewInfo* view = InitView();
    ResizeView(view, 0, 0);
    UpdateView(view);
    FinishView(view);
    return 0;
}

void InitGLExt()
{
#define GL_DECL(fun, decl) fun = (decl)wglGetProcAddress(#fun);
#include "glext_loader.inc"
#undef GL_DECL
}

#define GL_DECL(fun, decl) decl fun;
#include "glext_loader.inc"
#undef GL_DECL
