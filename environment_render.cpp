#include "environment.h"
#include "obj_load.h"
#include "simple_shader.h"
#include "simple_texture.h"
#include "vectormath.h"
#include "collections.h"
#include "camera.h"
#include "reporting.h"

#include "gl_all.h"
#include <stdio.h>
#include <assert.h>
#include <cstdio>
#include "system.h"

GLuint gQuad;
VertexDef gQuadVerts;


// Move this somewhere common.
void InitQuad();
void DestroyQuad();
void RenderUnitQuad();

struct EnvLayer
{
    float parallax;
    float aspect;
    GLuint layer_texture;

    Vector4 color_mask;
};

struct Environment
{
    GLuint environment_shader;
    GLuint environment_tex_uniform;
    GLuint color_mask_uniform;
    GLuint depth_uniform;
    GLuint local_to_world_mat_uniform;
    StaticArray<EnvLayer*> Layers;
    EnvLayer* bg_layer;
};

EnvLayer* InitEnvLayer(const char* texture_path, float parallax)
{
    EnvLayer* layer = new EnvLayer();

    layer->parallax = parallax;

    layer->color_mask = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

    bitmap* img = load_bmp(texture_path);
    layer->aspect = (float)bitmap_width(img) / (float)bitmap_height(img);
    layer->layer_texture = CreateTexture(img);
    destroy_bitmap(img);

    return layer;
}

void DestroyEnvLayer(EnvLayer* layer)
{
    DestroyTexture(layer->layer_texture);
    delete layer;
}

void RenderEnvLayer(EnvLayer* layer, Environment* e, Camera* camera)
{
    // This is all a bit of a mess, Maybe we just put everything in world space?
    float parallax_factor = 1.0f - layer->parallax;
    parallax_factor *= parallax_factor;
    float camera_aspect = CameraGetAspectRatio(camera);
    Vector3 camera_location = CameraGetLocation(camera);
    Matrix4 world_to_proj = CameraGetWorldToProjection(camera);
    Vector4 tmp = world_to_proj * camera_location;
    tmp /= tmp.getW();
    camera_location[0] = tmp[0];
    camera_location[1] = tmp[1];
    camera_location[2] = tmp[2];
    camera_location.setZ(0.0f);

    float aspect = layer->aspect/camera_aspect;

    Vector3 bg_offset((aspect - 1.0f), 0.0, 0.0);

    Matrix4 local_to_world =
        Matrix4::translation(bg_offset + camera_location * parallax_factor) *
        Matrix4::scale(Vector3(aspect, 1, 1));

    glUniformMatrix4fv(e->local_to_world_mat_uniform, 1, false, (float*)&local_to_world);
    glUniform1i(e->environment_tex_uniform, 0);
    glUniform4f(e->color_mask_uniform, layer->color_mask.getX(), layer->color_mask.getY(), layer->color_mask.getZ(), layer->color_mask.getW());
    glUniform1f(e->depth_uniform, layer->parallax);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, layer->layer_texture);

    RenderUnitQuad();
}

Environment* InitEnvironment(unsigned int width, unsigned int height)
{
    InitQuad();

    Environment* e = new Environment();

    e->environment_shader = CreateShaderProgram(SHADER_ENVIRONMENT);

    e->environment_tex_uniform = glGetUniformLocation(e->environment_shader, "environment_tex");
    e->color_mask_uniform = glGetUniformLocation(e->environment_shader, "color_mask");
    e->depth_uniform = glGetUniformLocation(e->environment_shader, "depth");
    e->local_to_world_mat_uniform = glGetUniformLocation(e->environment_shader, "local_to_world");

    VertexDefBindToShader(gQuadVerts, e->environment_shader);

    // Init layers
    e->Layers.reinit(5);
    for(unsigned int i = 0; i < e->Layers.size(); i++)
    {
        char path[255];
        snprintf(path, sizeof(path), "data/world/%i.bmp", i+1);

        float parallax = (float)i / (float)(e->Layers.size() - 1);

        e->Layers[i] = InitEnvLayer(path, parallax);
    }

    e->bg_layer = InitEnvLayer("data/world/BG.bmp", 1.0f);

    e->bg_layer->color_mask = Vector4(1.0f, 1.0f, 1.0f, 0.0f);

    return e;
}

void DestroyEnvironment(Environment* e)
{

    DestroyEnvLayer(e->bg_layer);

    for(unsigned int i = 0; i < e->Layers.size(); i++)
    {
        DestroyEnvLayer(e->Layers[i]);
    }
    e->Layers.clear();

    DestroyProgramAndAttachedShaders(e->environment_shader);
    delete e;
    DestroyQuad();
}

void RenderEnvironment(Environment* e, Camera* camera)
{
    glUseProgram(e->environment_shader);

    Matrix4 identity = Matrix4::identity();

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((float*)&identity);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((float*)&identity);


    glBlendFunc(GL_ONE, GL_ZERO);
    RenderEnvLayer(e->bg_layer, e, camera);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for(unsigned int i = e->Layers.size(); i > 0; i--)
    {
        RenderEnvLayer(e->Layers[i-1], e, camera);
    }
}

struct tex_quad_vert
{
    vector3 location;
    vector2 uv;
};

VertexDef gen_tex_quad_vert_def()
{
    tex_quad_vert* proxy = 0;
    VertexDef VD = CreateVertexDef(sizeof(tex_quad_vert), 2);
    int i = 0;
    AddVertexAttribute(VD, i++, VERTEX_POSITION_ATTR, (size_t)&proxy->location, 3, GL_FLOAT);
    AddVertexAttribute(VD, i++, VERTEX_UV_ATTR, (size_t)&proxy->uv, 2, GL_FLOAT);

    return VD;
}

const tex_quad_vert quad_mesh[] =
{{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
 {{1.0f, -1.0f, 0.0f},  {1.0f, 0.0f}},
 {{-1.0f, 1.0f, 0.0f},  {0.0f, 1.0f}},
 {{-1.0f, 1.0f, 0.0f},  {0.0f, 1.0f}},
 {{1.0f, -1.0f, 0.0f},  {1.0f, 0.0f}},
 {{1.0f, 1.0f, 0.0f},   {1.0f, 1.0f}}};

void InitQuad()
{
    gQuadVerts = gen_tex_quad_vert_def();
    gQuad = CreateMesh(6, sizeof(tex_quad_vert), quad_mesh);
}

void DestroyQuad()
{
    DestroyMesh(gQuad);
    DestroyVertexDef(gQuadVerts);
}

void RenderUnitQuad()
{
    glBindBuffer(GL_ARRAY_BUFFER, gQuad);
    ApplyVertexDef(gQuadVerts);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    ClearVertexDef(gQuadVerts);
}

