// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include <stdio.h>
#include "view_decl.h"
#include "simple_shader.h"
#include "simple_vectors.h"
#include "simple_mesh.h"
#include "obj_load.h"
#include "input.h"
#include "core_systems.h"

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

    VertexDef collision_vert;
    obj_mesh* collision_mesh;
    GLuint collision_gl_mesh;
    GLuint collision_mesh_indices;
    
    bool bMouseDown;

    PlayerInput* player_input;
};

ViewInfo* InitView()
{
    ViewInfo* view = new ViewInfo();

    init_sdl_system();

    view->player_input = InitPlayerInput();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    view->basic_shader = CreateShaderProgram(SHADER_CONSTANT_COLOR);
    view->diffuse_color_uniform = glGetUniformLocation(view->basic_shader,
                                                       "diffuse_color");

    glEnableClientState(GL_VERTEX_ARRAY);

    view->boot_vert = boot_vert_def();

    InitEditor();
    view->grid = CreateGridMesh(21, 0.1);

    view->bMouseDown = false;

    view->collision_vert = obj_vert_def();
    view->collision_mesh = obj_load_mesh("data/collision.obj");
    view->collision_gl_mesh = CreateMesh(view->collision_mesh->vertex_count,
                                         sizeof(obj_vert),
                                         view->collision_mesh->verticies);

    for(unsigned int i=0; i<view->collision_mesh->vertex_count; i++)
        printf("Vertex: %f, %f, %f\n",
               view->collision_mesh->verticies[i].location.x,
               view->collision_mesh->verticies[i].location.y,
               view->collision_mesh->verticies[i].location.z);

    for(unsigned int i=0; i<view->collision_mesh->face_count; i++)
        printf("Face: %d, %d, %d\n",
               view->collision_mesh->faces[i].a,
               view->collision_mesh->faces[i].b,
               view->collision_mesh->faces[i].c);

    glGenBuffers(1, &view->collision_mesh_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, view->collision_mesh_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 view->collision_mesh->face_count * sizeof(obj_face),
                 view->collision_mesh->faces,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    

    return view;
}

void FinishView(ViewInfo* view)
{
    DestroyMesh(view->collision_gl_mesh);
    glDeleteBuffers(1, &view->collision_mesh_indices);
    delete view->collision_mesh;
    
    DestroyMesh(view->grid);
    CleanupEditor();

    DestroyVertexDef(view->boot_vert);

    DestroyPlayerInput(view->player_input);
    finalize_sdl_system();

    delete view;
}

void ResizeView(ViewInfo*, int, int)
{}

void UpdateView(ViewInfo* view)
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(view->basic_shader);
    glUniform4f(view->diffuse_color_uniform,
                160.0f / 255.0f, 0.0f, 1.0f, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, view->collision_gl_mesh);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, view->collision_mesh_indices);
    ApplyVertexDef(view->collision_vert);

    glDrawElements(GL_TRIANGLES,
                   view->collision_mesh->face_count*3,
                   GL_UNSIGNED_SHORT, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glUniform4f(view->diffuse_color_uniform,
                1.0f, 0.0f, 0.0f, 1.0f);

    DrawEditorMesh(view->grid);
}

void MouseDown(ViewInfo* view)
{
    view->bMouseDown = true;
}
