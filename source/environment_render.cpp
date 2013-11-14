// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
//
// "If you want to build a ship, don't drum up the people to gather wood,
// divide the work, and give orders. Instead, teach them to yearn for the
// vast and endless sea." -- Antoine De Saint-Exupery
//
#include "environment.h"
#include "obj_load.h"
#include "simple_shader.h"
#include "simple_texture.h"
#include "uniforms.h"
#include "render_util.h"
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

struct EnvLayer
{
    float parallax;
    float height;
    float aspect;
    GLuint layer_texture;

    Vector4 color_mask;
};

REFLECT_TYPE(EnvLayer)
{
    reflect(&EnvLayer::parallax, "Depth");
    //reflect(&EnvLayer::layer_texture, "Texture");
    //reflect(&EnvLayer::color_mask, "ColorMask");
}

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
    float out_scattering;
    float rayleigh;
    float mie;
    float mie_eccentricity;

    Uniforms uniforms;

    static const unsigned int uniform_count = 15;

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
        out_scattering(0.0),
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
        uniforms.add_uniform("out_scattering", &out_scattering, i++, shader);
        uniforms.add_uniform("rayleigh", &rayleigh, i++, shader);
        uniforms.add_uniform("mie", &mie, i++, shader);
        uniforms.add_uniform("mie_eccentricity", &mie_eccentricity, i++, shader);
        uniforms.add_uniform("color_buffer", NULL, Uniform_Texture, i++, shader);
        uniforms.add_uniform("depth_buffer", NULL, Uniform_Texture, i++, shader);
        assert(i == uniforms.num_uniforms);
    }
};

REFLECT_TYPE(Vector3)
{
    // So bad!
    reflect.reflect((float*)0, "X");
    reflect.reflect((float*)4, "Y");
    reflect.reflect((float*)8, "Z");
}

REFLECT_TYPE(Scattering)
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
    reflect(&Scattering::out_scattering, "Outscattering");
    reflect(&Scattering::rayleigh, "Rayleigh");
    reflect(&Scattering::mie, "Mie");
    reflect(&Scattering::mie_eccentricity, "MieEccentricity");
}

struct Environment
{
    GLuint environment_shader;
    GLuint environment_tex_uniform;
    GLuint color_mask_uniform;
    GLuint depth_uniform;
    GLuint local_to_world_mat_uniform;

    StaticArray<EnvLayer*> Layers;

    unsigned int num_layers;
    float screen_height;
    Scattering scattering;

    Vector2 bounds;

    Environment();
};

REFLECT_TYPE(Environment)
{
    //reflect(&Environment::Layers, "Layers");
    reflect(&Environment::num_layers, "NumLayers");
    reflect(&Environment::screen_height, "ScreenHeight");
    reflect(&Environment::scattering, "Scattering");
    reflect(&Environment::bounds, "BoundsPx");
}

Environment::Environment() : bounds(0, 0, 0)
{
}

Reflect* GetEnvironmentReflection()
{
    return get_reflection<Environment>();
}

Uniforms* GetScatteringUniforms(Environment* e)
{
    return &e->scattering.uniforms;
}

void CaptureScatteringUniforms(Environment* e, GLuint shader)
{
    e->scattering.capture_uniforms(shader);
}

EnvLayer* InitEnvLayer(const char* texture_path, float parallax, float screen_height)
{
    EnvLayer* layer = new EnvLayer();

    layer->parallax = parallax;

    layer->color_mask = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

    bitmap* img = load_bmp(texture_path);
    // We're targeting a 1080p window height.
    layer->height = (float)bitmap_height(img) / screen_height;
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

Environment* InitEnvironment(const char* world, Camera* camera)
{
    char path[255];
    snprintf(path, sizeof(path), "data/world/%s", world);

    char tmp_path[255];
    snprintf(tmp_path, sizeof(tmp_path), "%s/world.json", path);

    Environment* e = persist_create_from_config<Environment>(tmp_path);

    e->bounds /= e->screen_height;
    e->bounds.setZ(0.0f);

    CameraSetBounds(camera, e->bounds);

    e->environment_shader = CreateShaderProgram(SHADER_ENVIRONMENT);

    e->environment_tex_uniform = glGetUniformLocation(e->environment_shader, "environment_tex");
    e->color_mask_uniform = glGetUniformLocation(e->environment_shader, "color_mask");
    e->depth_uniform = glGetUniformLocation(e->environment_shader, "depth");
    e->local_to_world_mat_uniform = glGetUniformLocation(e->environment_shader, "local_to_world");

    BindShaderToUnitQuad(e->environment_shader);

    unsigned int num_layers = e->num_layers;
    // Init layers
    e->Layers.reinit(num_layers + 1);
    for(unsigned int i = 0; i < num_layers; i++)
    {
        char layer_path[255];
        snprintf(layer_path, sizeof(layer_path), "%s/%i.bmp", path, i+1);

        float parallax = (float)i / (float)(num_layers - 1);

        e->Layers[i] = InitEnvLayer(layer_path, parallax, e->screen_height);
    }

    snprintf(tmp_path, sizeof(tmp_path), "%s/BG.bmp", path);
    e->Layers[num_layers] = InitEnvLayer(tmp_path, 1.0f, e->screen_height);

    e->Layers[num_layers]->color_mask = Vector4(1.0f, 1.0f, 1.0f, 0.0f);

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
