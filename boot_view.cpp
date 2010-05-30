// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include "view_decl.h"
#include "simple_shader.h"
#include <gl.h>

// Example view implementation.

struct ViewInfo
{
    GLuint basic_shader;
    GLint diffuse_color_uniform;
};

ViewInfo* InitView()
{
    ViewInfo* view = new ViewInfo();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    view->basic_shader = CreateShaderProgram(SHADER_CONSTANT_COLOR);
    view->diffuse_color_uniform = glGetUniformLocation(view->basic_shader,
                                                       "diffuse_color");
    return view;
}

void FinishView(ViewInfo* view)
{
    delete view;
}

void ResizeView(ViewInfo*, int width, int height)
{}

void UpdateView(ViewInfo* view)
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(view->basic_shader);
    glUniform4f(view->diffuse_color_uniform,
                0.0f, 1.0f, 0.0f, 1.0f);

    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 1.0, 0.0);
    glVertex3f(-1.0, -1.0, 0.0);
    glVertex3f(1.0, -1.0, 0.0);
    glEnd();
}
