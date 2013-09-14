#include "environment.h"
#include "obj_load.h"
#include "simple_shader.h"
#include "simple_texture.h"
#include "vectormath.h"
#include "camera.h"

#include "gl_all.h"
#include <stdio.h>
#include <assert.h>
#include <cstdio>
#include "system.h"

GLuint gQuad;
VertexDef gQuadVerts;

void InitQuad();
void DestroyQuad();
void RenderUnitQuad();

struct EnvLayer
{
    GLuint layer_texture;
};

struct Environment
{
    GLuint environment_shader;
    GLuint environment_tex_uniform;
    unsigned int NumLayers;
    EnvLayer** Layers;
};

EnvLayer* InitEnvLayer(const char* texture_path)
{
    EnvLayer* layer = new EnvLayer();
    layer->layer_texture = CreateTextureFromBMP(texture_path);

    return layer;
}

void DestroyEnvLayer(EnvLayer* layer)
{
    DestroyTexture(layer->layer_texture);
    delete layer;
}

void RenderEnvLayer(EnvLayer* layer, Environment* e)
{
    glUniform1i(e->environment_tex_uniform, 0);
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

    VertexDefBindToShader(gQuadVerts, e->environment_shader);

    // Init layers
    e->NumLayers = 5;
    e->Layers = new EnvLayer*[e->NumLayers];
    for(unsigned int i = 0; i < e->NumLayers; i++)
    {
        char path[255];
        snprintf(path, sizeof(path), "data/world/%i.bmp", i+1);
        e->Layers[i] = InitEnvLayer(path);
    }

    return e;
}

void DestroyEnvironment(Environment* e)
{

    for(unsigned int i = 0; i < e->NumLayers; i++)
    {
        DestroyEnvLayer(e->Layers[i]);
    }
    delete[] e->Layers;

    DestroyProgramAndAttachedShaders(e->environment_shader);
    delete e;
    DestroyQuad();
}

void RenderEnvironment(Environment* e)
{
    glUseProgram(e->environment_shader);

    Matrix4 identity = Matrix4::identity();

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((float*)&identity);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((float*)&identity);

    for(unsigned int i = e->NumLayers; i > 0; i--)
    {
        RenderEnvLayer(e->Layers[i-1], e);
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

