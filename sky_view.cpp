// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include <stdio.h>
#include "view_decl.h"
#include "simple_shader.h"
#include "simple_vectors.h"
#include "simple_texture.h"
#include "simple_mesh.h"
#include "obj_load.h"
#include "input.h"
#include "core_systems.h"

#include "character.h"
#include "camera.h"

#include "editor_meshes.h"

#include <assert.h>
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

const int Num_Players = 2;

struct ViewInfo
{
    GLuint basic_shader;
    GLint diffuse_color_uniform;
    VertexDef boot_vert;
    GLuint test_mesh;

    Editor_Mesh* grid;

    GLuint environment_shader;

    VertexDef collision_vert;
    obj_mesh* collision_mesh;
    GLuint collision_gl_mesh;
    
    bool bMouseDown;

    PlayerInput* player_input[Num_Players];
    Character* character;

    Camera* camera;
};

ViewInfo* InitView()
{
    ViewInfo* view = new ViewInfo();

    init_sdl_system();

    int num_inputs = InitPlayerInputs(view->player_input, Num_Players);
    
    assert(num_inputs >= 1);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    view->basic_shader = CreateShaderProgram(SHADER_CONSTANT_COLOR);
    view->diffuse_color_uniform = glGetUniformLocation(view->basic_shader,
                                                       "diffuse_color");

    view->environment_shader = CreateShaderProgram(SHADER_ENVIRONMENT);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    view->boot_vert = boot_vert_def();

    InitEditor();
    view->grid = CreateGridMesh(21, 1);

    view->bMouseDown = false;

    view->collision_vert = obj_vert_def();
    view->collision_mesh = obj_load_mesh("data/world/core_world.obj");
    view->collision_gl_mesh = CreateMesh(view->collision_mesh->vertex_count,
                                         sizeof(obj_vert),
                                         view->collision_mesh->verticies);

    /*for(unsigned int i=0; i<view->collision_mesh->vertex_count; i++)
        printf("Vertex: %f, %f, %f\nColor: %f %f %f\n",
               view->collision_mesh->verticies[i].location.x,
               view->collision_mesh->verticies[i].location.y,
               view->collision_mesh->verticies[i].location.z,
               view->collision_mesh->verticies[i].diffuse.x,
               view->collision_mesh->verticies[i].diffuse.y,
               view->collision_mesh->verticies[i].diffuse.z);*/

    /*for(unsigned int i=0; i<view->collision_mesh->face_count; i++)
        printf("Face: %d, %d, %d\n",
               view->collision_mesh->faces[i].a,
               view->collision_mesh->faces[i].b,
               view->collision_mesh->faces[i].c);*/


    view->camera = InitCamera(1024, 768);

    InitCharacters();
    view->character = CreateCharacter(view->player_input[0]);
    

    return view;
}

void FinishView(ViewInfo* view)
{
    DestroyCharacter(view->character);
    FinishCharacters();

    DestroyCamera(view->camera);

    DestroyMesh(view->collision_gl_mesh);
    delete view->collision_mesh;
    
    DestroyMesh(view->grid);
    CleanupEditor();

    DestroyVertexDef(view->boot_vert);

    DestroyPlayerInputs(view->player_input, Num_Players);
    finalize_sdl_system();

    delete view;
}

void ResizeView(ViewInfo* view, int width, int height)
{
    SetCameraProjection(view->camera, width, height);
}

void UpdateView(ViewInfo* view)
{
    for(int i=0; i<Num_Players; i++)
        if(view->player_input[i])
            UpdateInput(view->player_input[i]);

    UpdateCharacter(view->character, 0.02f);

    Vector3 char_location = GetCharacterLocation(view->character);

    UpdateCamera(view->camera, &char_location, 1);

    glClear(GL_COLOR_BUFFER_BIT);

    Matrix4 view_projection = CameraGetWorldToProjection(view->camera);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((float*)&view_projection);

    glUseProgram(view->basic_shader);

    glUniform4f(view->diffuse_color_uniform,
                0.25f, 0.0f, 0.0f, 1.0f);
    DrawEditorMesh(view->grid);

    glUseProgram(view->environment_shader);

    //glUniform4f(view->diffuse_color_uniform,
    //            160.0f / 255.0f, 0.0f, 1.0f, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, view->collision_gl_mesh);
    ApplyVertexDef(view->collision_vert);

    glDrawArrays(GL_TRIANGLES, 0,
                 view->collision_mesh->vertex_count);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    RenderCharacter(view->character);
}

void MouseDown(ViewInfo* view)
{
    view->bMouseDown = true;
}
