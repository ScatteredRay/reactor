#include "environment.h"
#include "obj_load.h"
#include "simple_shader.h"
#include "simple_texture.h"
#include "vectormath.h"
#include "collections.h"
#include "camera.h"
#include "reporting.h"
#include "persist.h"

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
    float height;
    float aspect;
    GLuint layer_texture;

    Vector4 color_mask;
};

template <>
struct Reflect_Type<EnvLayer>
{
    static void metadata(Reflect& reflect)
    {
        reflect(&EnvLayer::parallax, "Depth");
        //reflect(&EnvLayer::layer_texture, "Texture");
        //reflect(&EnvLayer::color_mask, "ColorMask");
    }
};

struct Environment
{
    GLuint environment_shader;
    GLuint environment_tex_uniform;
    GLuint color_mask_uniform;
    GLuint depth_uniform;
    GLuint local_to_world_mat_uniform;
    StaticArray<EnvLayer*> Layers;
    int X;
    float Y;
    int* Z;
    bool W;
    enum {my_enum} F;
    struct {} G;
    int H[3][4];
};

template <>
struct Reflect_Type<Environment>
{
    static void metadata(Reflect& reflect)
    {
        //reflect(&Environment::Layers, "Layers");
        reflect(&Environment::X, "X");
        reflect(&Environment::Y, "Y");
        //reflect(&Environment::Z, "Z");
        reflect(&Environment::W, "W");
        //reflect(&Environment::F, "F");
        //reflect(&Environment::G, "G");
        //reflect(&Environment::H, "H");
    }
};

EnvLayer* InitEnvLayer(const char* texture_path, float parallax)
{
    EnvLayer* layer = new EnvLayer();

    layer->parallax = parallax;

    layer->color_mask = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

    bitmap* img = load_bmp(texture_path);
    // We're targeting a 1080p window height.
    layer->height = (float)bitmap_height(img) / 1080.0f;
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

    float height = layer->height;

    float aspect = layer->aspect/camera_aspect;

    Vector3 bg_offset((aspect * height) - 1.0f, height - 1.0f, 0.0);

    Matrix4 local_to_world =
        Matrix4::translation(bg_offset + camera_location * parallax_factor) *
        Matrix4::scale(Vector3(aspect * height, height, 1));

    glUniformMatrix4fv(e->local_to_world_mat_uniform, 1, false, (float*)&local_to_world);
    glUniform1i(e->environment_tex_uniform, 0);
    glUniform4f(e->color_mask_uniform, layer->color_mask.getX(), layer->color_mask.getY(), layer->color_mask.getZ(), layer->color_mask.getW());
    glUniform1f(e->depth_uniform, layer->parallax);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, layer->layer_texture);

    RenderUnitQuad();
}

Environment* InitEnvironment()
{
    InitQuad();

    Environment* e = persist_create_from_config<Environment>("data/world/world.json");

    e->environment_shader = CreateShaderProgram(SHADER_ENVIRONMENT);

    e->environment_tex_uniform = glGetUniformLocation(e->environment_shader, "environment_tex");
    e->color_mask_uniform = glGetUniformLocation(e->environment_shader, "color_mask");
    e->depth_uniform = glGetUniformLocation(e->environment_shader, "depth");
    e->local_to_world_mat_uniform = glGetUniformLocation(e->environment_shader, "local_to_world");

    VertexDefBindToShader(gQuadVerts, e->environment_shader);

    // Init layers
    unsigned int num_layers = 6;
    e->Layers.reinit(num_layers);
    for(unsigned int i = 0; i < num_layers-1; i++)
    {
        char path[255];
        snprintf(path, sizeof(path), "data/world/%i.bmp", i+1);

        float parallax = (float)i / (float)(num_layers - 1);

        e->Layers[i] = InitEnvLayer(path, parallax);
    }

    e->Layers[num_layers-1] = InitEnvLayer("data/world/BG.bmp", 1.0f);

    e->Layers[num_layers-1]->color_mask = Vector4(1.0f, 1.0f, 1.0f, 0.0f);

    return e;
}

void DestroyEnvironment(Environment* e)
{

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
    unsigned int num_layers = e->Layers.size();
    for(unsigned int i = num_layers; i > 0; i--)
    {
        RenderEnvLayer(e->Layers[i-1], e, camera);
        if(i == num_layers)
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
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

