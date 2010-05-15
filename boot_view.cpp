// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include "view_decl.h"
#include "gl.h"

// Example view implementation.

struct ViewInfo{};

ViewInfo* InitView()
{
    return 0;
}

void FinishView(ViewInfo*)
{}

void ResizeView(ViewInfo*, int width, int height)
{}

void UpdateView(ViewInfo*)
{
    static float r = 1.0f;
    glClearColor(r, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if(r == 1.0f) r = 0.0f;
    else r = 1.0f;
}
