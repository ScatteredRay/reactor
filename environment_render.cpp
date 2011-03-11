#include "environment.h"
#include "obj_load.h"
#include "simple_shader.h"
#include "simple_mesh.h"
#include "vectormath.h"
#include "camera.h"

#include <gl.h>
#include <glext.h>
#include <stdio.h>
#include <assert.h>

const unsigned int ENV_NUM_FRAMEBUFFERS = 5;

struct Environment
{
    GLuint environment_shader;
    VertexDef environment_vert;
    obj_mesh* environment_mesh;
    GLuint environment_gl_mesh;
    GLuint framebuffer;
    GLuint depth_render_buffer;
    GLuint framebuffer_textures[ENV_NUM_FRAMEBUFFERS];
    GLenum render_buffer_list[ENV_NUM_FRAMEBUFFERS];
};

void InitRenderBuffers(Environment* e, unsigned int width, unsigned int height)
{
    glGenFramebuffers(1, &e->framebuffer);
    glGenTextures(ENV_NUM_FRAMEBUFFERS, e->framebuffer_textures);

    glBindFramebuffer(GL_FRAMEBUFFER, e->framebuffer);


    glGenRenderbuffers(1, &e->depth_render_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, e->depth_render_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, e->depth_render_buffer);

    for(unsigned int i=0; i<ENV_NUM_FRAMEBUFFERS; i++)
    {
        glBindTexture(GL_TEXTURE_2D, e->framebuffer_textures[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


        // Hopefully GL_COLOR_ATTACHMENTX are all lined up.

        // Can we just use float buffers? Boy would that be fun!
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
                               e->framebuffer_textures[i],
                               0);

        e->render_buffer_list[i] = GL_COLOR_ATTACHMENT0 + i;
    }

    glDrawBuffers(ENV_NUM_FRAMEBUFFERS, e->render_buffer_list);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(status == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DestroyRenderBuffers(Environment* e)
{
    glDeleteRenderbuffers(1, &e->depth_render_buffer);
    glDeleteTextures(ENV_NUM_FRAMEBUFFERS, e->framebuffer_textures);
    glDeleteFramebuffers(1, &e->framebuffer);
}

void ResizeRenderBuffers(Environment* e, unsigned int width, unsigned int height)
{
    DestroyRenderBuffers(e);
    InitRenderBuffers(e, width, height);
}

Environment* InitEnvironment(unsigned int width, unsigned int height)
{
    GLint maxbuffers;
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxbuffers);
    printf("Max supported draw buffers: %d\n", maxbuffers);
    assert(maxbuffers >= (int)ENV_NUM_FRAMEBUFFERS);

    GLint maxattribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxattribs);
    printf("Max supported vertex attributes: %d\n", maxattribs);

    Environment* e = new Environment();

    e->environment_shader = CreateShaderProgram(SHADER_ENVIRONMENT);

    // Remember glEnableVertexAttribArray
    glBindAttribLocation(e->environment_shader, 1, "ambient");
    glBindAttribLocation(e->environment_shader, 2, "specular");
    glBindAttribLocation(e->environment_shader, 3, "emissive");

    // Needs to be called to actually set the attib locations.
    glLinkProgram(e->environment_shader);

    e->environment_vert = obj_vert_def();
    e->environment_mesh = obj_load_mesh("data/world/core_world.obj");
    e->environment_gl_mesh = CreateMesh(e->environment_mesh->vertex_count,
                                        sizeof(obj_vert),
                                        e->environment_mesh->verticies);

    InitRenderBuffers(e, width, height);

    return e;
}

void DestroyEnvironment(Environment* e)
{
    DestroyRenderBuffers(e);

    DestroyMesh(e->environment_gl_mesh);
    DestroyVertexDef(e->environment_vert);

    DestroyProgramAndAttachedShaders(e->environment_shader);

    delete e->environment_mesh;
}

void RenderEnvironment(Environment* e)
{
    glBindFramebuffer(GL_FRAMEBUFFER, e->framebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(e->environment_shader);

    glBindBuffer(GL_ARRAY_BUFFER, e->environment_gl_mesh);
    ApplyVertexDef(e->environment_vert);
    
    glDrawArrays(GL_TRIANGLES, 0, e->environment_mesh->vertex_count);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
 {{-1.0f, 1.0f, 0.0f},  {0.0f, 1.0f}},
 {{1.0f, -1.0f, 0.0f},  {1.0f, 0.0f}},
 {{-1.0f, 1.0f, 0.0f},  {0.0f, 1.0f}},
 {{1.0f, -1.0f, 0.0f},  {1.0f, 0.0f}},
 {{1.0f, 1.0f, 0.0f},   {1.0f, 1.0f}}};

GLuint gQuad;
VertexDef gQuadVerts;

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
}

struct DeferredRender
{
    GLuint deferred_lighting_shader;
    GLuint color_buffer_uniform;
    GLuint normal_buffer_uniform;
    GLuint ambient_buffer_uniform;
    GLuint specular_buffer_uniform;
    GLuint emissive_buffer_uniform;

    GLuint directional_light_uniform;
    GLuint camera_location_uniform;

    GLuint inv_projection_uniform;
};

DeferredRender* InitDeferred()
{
    InitQuad();
    DeferredRender* r = new DeferredRender();
    
    r->deferred_lighting_shader = CreateShaderProgram(SHADER_DEFERRED_LIGHTING);
    r->color_buffer_uniform = glGetUniformLocation(r->deferred_lighting_shader, "color_buffer");
    r->normal_buffer_uniform = glGetUniformLocation(r->deferred_lighting_shader, "normal_buffer");
    r->ambient_buffer_uniform = glGetUniformLocation(r->deferred_lighting_shader, "ambient_buffer");
    r->specular_buffer_uniform = glGetUniformLocation(r->deferred_lighting_shader, "specular_buffer");
    r->emissive_buffer_uniform = glGetUniformLocation(r->deferred_lighting_shader, "emissive_buffer");
    r->directional_light_uniform = glGetUniformLocation(r->deferred_lighting_shader, "directional_light");
    r->camera_location_uniform = glGetUniformLocation(r->deferred_lighting_shader, "camera_location");
    r->inv_projection_uniform = glGetUniformLocation(r->deferred_lighting_shader, "inv_projection");
    return r;
}

void DestroyDeferred(DeferredRender* r)
{
    DestroyProgramAndAttachedShaders(r->deferred_lighting_shader);
    delete r;
    DestroyQuad();
}

void RenderDeferred(DeferredRender* r, Environment* e, Camera* camera)
{
    glUseProgram(r->deferred_lighting_shader);

    //Clean me up!

    glUniform1i(r->color_buffer_uniform, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, e->framebuffer_textures[0]);

    glUniform1i(r->normal_buffer_uniform, 1);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, e->framebuffer_textures[1]);

    glUniform1i(r->ambient_buffer_uniform, 2);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, e->framebuffer_textures[2]);

    glUniform1i(r->specular_buffer_uniform, 3);
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, e->framebuffer_textures[3]);

    glUniform1i(r->emissive_buffer_uniform, 4);
    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_2D, e->framebuffer_textures[4]);

    Vector3 light_dir(0.5f, 0.1f, 1.0f);

    glUniform3f(r->directional_light_uniform,
                light_dir.getX(),
                light_dir.getY(),
                light_dir.getZ());

    Vector3 camera_location = CameraGetLocation(camera);

    glUniform3f(r->camera_location_uniform,
                camera_location.getX(),
                camera_location.getY(),
                camera_location.getZ());

    Matrix4 invprojection = inverse(CameraGetProjection(camera));

    glUniformMatrix4fv(r->inv_projection_uniform,
                       1,
                       false,
                       (float*)&invprojection);
    
    RenderUnitQuad();

    glBindTexture(GL_TEXTURE_2D, 0);
}
