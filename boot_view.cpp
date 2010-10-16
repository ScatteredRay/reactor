// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include "view_decl.h"
#include "simple_shader.h"
#include "simple_vectors.h"
#include "simple_mesh.h"

#include "editor_meshes.h"
#include <gl.h>

struct boot_vert
{
    vector3 location;
    vector3 normal;
    vector4 color;
};

VertexDef boot_vert_def()
{
    boot_vert* proxy = 0;
    VertexDef VD = CreateVertexDef(sizeof(boot_vert), 3);
    int i = 0;
    AddVertexAttribute(VD, i++, VERTEX_POSITION_ATTR, (size_t)&proxy->location, 3, GL_FLOAT);
    AddVertexAttribute(VD, i++, VERTEX_NORMAL_ATTR, (size_t)&proxy->normal, 3, GL_FLOAT);
    AddVertexAttribute(VD, i++, VERTEX_COLOR_ATTR, (size_t)&proxy->color, 4, GL_FLOAT);

    return VD;
}

// Example view implementation.

struct ViewInfo
{
    GLuint basic_shader;
    GLint diffuse_color_uniform;
    VertexDef boot_vert;
    GLuint test_mesh;
    Editor_Mesh* grid;
};

ViewInfo* InitView()
{
    ViewInfo* view = new ViewInfo();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    view->basic_shader = CreateShaderProgram(SHADER_CONSTANT_COLOR);
    view->diffuse_color_uniform = glGetUniformLocation(view->basic_shader,
                                                       "diffuse_color");

    glEnableClientState(GL_VERTEX_ARRAY);

    view->boot_vert = boot_vert_def();

    boot_vert verts[3];
    verts[0].location.x = 0.0;
    verts[0].location.y = 1.0;
    verts[0].location.z = 0.0;

    verts[1].location.x = -1.0;
    verts[1].location.y = -1.0;
    verts[1].location.z = 0.0;

    verts[2].location.x = 1.0;
    verts[2].location.y = -1.0;
    verts[2].location.z = 0.0;

    view->test_mesh = CreateMesh(3, sizeof(boot_vert), verts);

    InitEditor();
    view->grid = CreateGridMesh(10, 0.1);

    return view;
}

void FinishView(ViewInfo* view)
{
    DestroyMesh(view->grid);
    CleanupEditor();

    DestroyMesh(view->test_mesh);
    DestroyVertexDef(view->boot_vert);
    delete view;
}

void ResizeView(ViewInfo*, int, int)
{}

void UpdateView(ViewInfo* view)
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(view->basic_shader);
    glUniform4f(view->diffuse_color_uniform,
    0.0f, 1.0f, 0.0f, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, view->test_mesh);
    ApplyVertexDef(view->boot_vert);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUniform4f(view->diffuse_color_uniform,
                1.0f, 0.0f, 0.0f, 1.0f);
    DrawEditorMesh(view->grid);
}
