#include "environment.h"
#include "obj_load.h"
#include "simple_shader.h"
#include "simple_texture.h"
#include "uniforms.h"
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

struct Scattering
{
    Vector3 light_source;
    Vector3 sun_color;
    float sun_power;
    float num_samples;
    float weight;
    float decay;
    float extinction;
    float ambient;
    float angular;
    float rayleigh;
    float mie;
    float mie_eccentricity;

    Uniforms uniforms;

    static const unsigned int uniform_count = 12;

    Scattering() :
        light_source(0.0, 0.0, 0.0),
        sun_color(0.0, 0.0, 0.0),
        sun_power(0.0),
        num_samples(0.0),
        weight(0.0),
        decay(0.0),
        extinction(0.0),
        ambient(0.0),
        angular(0.0),
        rayleigh(0.0),
        mie(0.0),
        mie_eccentricity(0.0),
        uniforms(uniform_count)
    {}

    void capture_uniforms(GLuint shader)
    {
        int i = 0;
        uniforms.add_uniform("light_source", &light_source, i++, shader);
        uniforms.add_uniform("sun_color", &sun_color, i++, shader);
        uniforms.add_uniform("sun_power", &sun_power, i++, shader);
        uniforms.add_uniform("num_samples", &num_samples, i++, shader);
        uniforms.add_uniform("weight", &weight, i++, shader);
        uniforms.add_uniform("decay", &decay, i++, shader);
        uniforms.add_uniform("extinction", &extinction, i++, shader);
        uniforms.add_uniform("ambient", &ambient, i++, shader);
        uniforms.add_uniform("angular", &angular, i++, shader);
        uniforms.add_uniform("rayleigh", &rayleigh, i++, shader);
        uniforms.add_uniform("mie", &mie, i++, shader);
        uniforms.add_uniform("mie_eccentricity", &mie_eccentricity, i++, shader);
        assert(i == uniform_count);
    }
};

template <>
struct Reflect_Type<Vector3>
{
    static void metadata(Reflect& reflect)
    {
        // So bad!
        reflect.reflect((float*)0, "X");
        reflect.reflect((float*)4, "Y");
        reflect.reflect((float*)8, "Z");
    }
};

template <>
struct Reflect_Type<Scattering>
{
    static void metadata(Reflect& reflect)
    {
        reflect(&Scattering::light_source, "LightLocation");
        reflect(&Scattering::sun_color, "LightColor");
        reflect(&Scattering::sun_power, "LightPower");
        reflect(&Scattering::num_samples, "NumSamples");
        reflect(&Scattering::weight, "Weight");
        reflect(&Scattering::decay, "Decay");
        reflect(&Scattering::extinction, "Extinction");
        reflect(&Scattering::ambient, "Ambient");
        reflect(&Scattering::angular, "Angular");
        reflect(&Scattering::rayleigh, "Rayleigh");
        reflect(&Scattering::mie, "Mie");
        reflect(&Scattering::mie_eccentricity, "MieEccentricity");
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
    Scattering scattering;
};

template <>
struct Reflect_Type<Environment>
{
    static void metadata(Reflect& reflect)
    {
        //reflect(&Environment::Layers, "Layers");
        reflect(&Environment::scattering, "Scattering");
    }
};

Uniforms* GetScatteringUniforms(Environment* e)
{
    return &e->scattering.uniforms;
}

void CaptureScatteringUniforms(Environment* e, GLuint shader)
{
    e->scattering.capture_uniforms(shader);
}

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

