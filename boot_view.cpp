// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include "view_decl.h"
#include "gl.h"

// Example view implementation.

struct ViewInfo{};

ViewInfo* InitView()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    return 0;
}

void FinishView(ViewInfo*)
{}

void ResizeView(ViewInfo*, int width, int height)
{}

void UpdateView(ViewInfo*)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 1.0, 0.0);
    glVertex3f(-1.0, -1.0, 0.0);
    glVertex3f(1.0, -1.0, 0.0);
    glEnd();
}
