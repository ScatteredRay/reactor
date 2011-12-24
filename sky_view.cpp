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
#include "environment.h"

#include "editor_meshes.h"

#include <assert.h>
#include "gl_all.h"

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

    VertexDef collision_vert;
    obj_mesh* collision_mesh;
    GLuint collision_gl_mesh;

    bool bMouseDown;

    PlayerInput* player_input[Num_Players];
    Character* character;

    Camera* camera;
    DeferredRender* deferred;

    Environment* environment;
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

    // Print out hardware info.
    GLint maxbuffers;
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxbuffers);
    printf("Max supported draw buffers: %d\n", maxbuffers);

    GLint maxattribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxattribs);
    printf("Max supported vertex attributes: %d\n", maxattribs);

    GLint maxvertex;
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &maxvertex);
    printf("Max supported element vertices: %d\n", maxvertex);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    view->boot_vert = boot_vert_def();

    InitEditor();
    view->grid = CreateGridMesh(21, 1);

    view->bMouseDown = false;

    view->collision_vert = obj_vert_def();
    view->collision_mesh = obj_load_mesh("data/world/collision.obj");
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

    view->deferred = InitDeferred();

    view->environment = InitEnvironment(1024, 768);

    return view;
}

void FinishView(ViewInfo* view)
{
    DestroyEnvironment(view->environment);
    DestroyDeferred(view->deferred);

    DestroyCharacter(view->character);
    FinishCharacters();

    DestroyCamera(view->camera);

    DestroyMesh(view->collision_gl_mesh);
    delete view->collision_mesh;
    
    DestroyMesh(view->grid);
    CleanupEditor();

    DestroyVertexDef(view->boot_vert);
    DestroyVertexDef(view->collision_vert);

    DestroyProgramAndAttachedShaders(view->basic_shader);

    DestroyPlayerInputs(view->player_input, Num_Players);
    finalize_sdl_system();

    delete view;
}

void ResizeView(ViewInfo* view, int width, int height)
{
    SetCameraProjection(view->camera, width, height);
    ResizeRenderBuffers(view->environment, width, height);
}

void UpdateView(ViewInfo* view)
{
    for(int i=0; i<Num_Players; i++)
        if(view->player_input[i])
            UpdateInput(view->player_input[i]);

    UpdateCharacter(view->character, 0.02f);

    Vector3 char_location = GetCharacterLocation(view->character);

    UpdateCamera(view->camera, &char_location, 1);

    Matrix4 modelview = CameraGetWorldToView(view->camera);
    Matrix4 projection = CameraGetProjection(view->camera);
    Matrix4 identity = Matrix4::identity();

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((float*)&modelview);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((float*)&projection);

    RenderEnvironment(view->environment);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((float*)&identity);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((float*)&identity);
    RenderDeferred(view->deferred, view->environment, view->camera);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((float*)&modelview);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((float*)&projection);

    glUseProgram(view->basic_shader);

    glUniform4f(view->diffuse_color_uniform,
                0.25f, 0.0f, 0.0f, 1.0f);
    DrawEditorMesh(view->grid);

    //glUniform4f(view->diffuse_color_uniform,
    //            160.0f / 255.0f, 0.0f, 1.0f, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, view->collision_gl_mesh);
    ApplyVertexDef(view->collision_vert);

    glDrawArrays(GL_TRIANGLES, 0,
                 view->collision_mesh->vertex_count);

    RenderCharacter(view->character);
}

void MouseDown(ViewInfo* view)
{
    view->bMouseDown = true;
}
