// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "particles.h"

#include "camera.h"
#include "simple_texture.h"
#include "simple_shader.h"
#include "simple_mesh.h"
#include "simple_vectors.h"
#include "render_target.h"
#include "render_util.h"
#include "uniforms.h"
#include "reporting.h"

#include "gl_all.h"
#include <assert.h>

struct GLArraysIndirectCommand
{
    unsigned int count;
    unsigned int primCount;
    unsigned int first;
    unsigned int baseInstance;
};

struct ParticleVert
{
    vector4 location;
    vector4 velocity;
};

VertexDef GenParticleVertDef()
{
    ParticleVert* proxy = 0;
    VertexDef VD = CreateVertexDef(sizeof(ParticleVert), 2);
    int i = 0;
    AddVertexAttribute(VD, i++, VERTEX_OTHER_ATTR, (size_t)&proxy->location, 4, GL_FLOAT, "in_Position");
    AddVertexAttribute(VD, i++, VERTEX_OTHER_ATTR, (size_t)&proxy->velocity, 4, GL_FLOAT, "in_Velocity");

    return VD;
}

struct Particles
{
    static const unsigned int BUFFER_COUNT = 2;
    unsigned int vertex_count;
    unsigned int frame_count;
    GLuint vertex_buffers[BUFFER_COUNT];
    GLuint vertex_textures[BUFFER_COUNT];
    GLuint atomic_count_buffer;

    GLuint shader_gen;
    Uniforms gen_uniforms;
    GLuint shader_sim;
    Uniforms sim_uniforms;

    VertexDef particle_vertex_def;

    unsigned int projection_uniform_gen;
    unsigned int inv_projection_uniform_gen;
    unsigned int projection_uniform_sim;
    unsigned int inv_projection_uniform_sim;
    unsigned int scene_depth_uniform;

    Particles() : gen_uniforms(6), sim_uniforms(4)
    {
    }

    void CaptureUniforms()
    {
        int i = 0;
        gen_uniforms.add_uniform("particle_buffer", &vertex_textures[0], Uniform_Image, i++, shader_gen);
        // TODO: This only works because `count` is the first element of `GLArraysIndirectCommand`. Make this more general.
        gen_uniforms.add_uniform("particle_count", &atomic_count_buffer, Uniform_Atomic, i++, shader_gen);
        gen_uniforms.add_uniform("max_particles", &vertex_count, i++, shader_gen);
        gen_uniforms.add_uniform("frame_count", &frame_count, i++, shader_gen);
        projection_uniform_gen = gen_uniforms.add_uniform("view_projection", NULL, Uniform_Mat4, i++, shader_gen);
        inv_projection_uniform_gen = gen_uniforms.add_uniform("inv_view_projection", NULL, Uniform_Mat4, i++, shader_gen);
        assert(i == gen_uniforms.num_uniforms);

        i = 0;
        sim_uniforms.add_uniform("particle_count", &atomic_count_buffer, Uniform_Atomic, i++, shader_sim);
        projection_uniform_sim = sim_uniforms.add_uniform("view_projection", NULL, Uniform_Mat4, i++, shader_sim);
        inv_projection_uniform_sim = sim_uniforms.add_uniform("inv_view_projection", NULL, Uniform_Mat4, i++, shader_sim);
        scene_depth_uniform = sim_uniforms.add_uniform("scene_depth", NULL, Uniform_Texture, i++, shader_sim);
        assert(i == sim_uniforms.num_uniforms);
    }

};

Particles* InitParticles()
{
    Particles* particles = new Particles();

    particles->frame_count = 0;

    particles->particle_vertex_def = GenParticleVertDef();

    particles->shader_gen = CreateShaderProgram(SHADER_PARTICLE_GEN);
    particles->shader_sim = CreateShaderProgram(SHADER_PARTICLE_SIM);
    BindShaderToUnitQuad(particles->shader_gen);
    const char* varyings[] = {"out_Position", "out_Velocity"};
    glTransformFeedbackVaryings(particles->shader_sim, 2, varyings, GL_INTERLEAVED_ATTRIBS);
    VertexDefBindToShader(particles->particle_vertex_def, particles->shader_sim);
    particles->CaptureUniforms();

    particles->vertex_count = 65536 * 10;
    unsigned int vertex_len = sizeof(ParticleVert);

    glGenBuffers(Particles::BUFFER_COUNT, particles->vertex_buffers);
    glGenTextures(Particles::BUFFER_COUNT, particles->vertex_textures);

    for(unsigned int i = 0; i < Particles::BUFFER_COUNT; i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, particles->vertex_buffers[i]);
        glBufferData(GL_ARRAY_BUFFER, vertex_len * particles->vertex_count, NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindTexture(GL_TEXTURE_BUFFER, particles->vertex_textures[i]);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, particles->vertex_buffers[i]);
        glBindBuffer(GL_TEXTURE_BUFFER, 0);
    }

    glGenBuffers(1, &particles->atomic_count_buffer);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, particles->atomic_count_buffer);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLArraysIndirectCommand), NULL, GL_DYNAMIC_DRAW);
    // Do we need to call BufferData if we just map it right after?
    GLArraysIndirectCommand* atomic_data = (GLArraysIndirectCommand*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLArraysIndirectCommand), GL_MAP_WRITE_BIT);
    atomic_data->count = 0;
    atomic_data->primCount = 1;
    atomic_data->first = 0;
    atomic_data->baseInstance = 0;
    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, NULL);

    return particles;
}

void DestroyParticles(Particles* particles)
{
    for(unsigned int i = 0; i < Particles::BUFFER_COUNT; i++)
    {
        glDeleteTextures(1, &particles->vertex_textures[i]);
        glDeleteBuffers(1, &particles->vertex_buffers[i]);
    }
    DestroyProgramAndAttachedShaders(particles->shader_gen);
    DestroyProgramAndAttachedShaders(particles->shader_sim);

    DestroyVertexDef(particles->particle_vertex_def);
    delete particles;
}

void UpdateParticles(Particles* particles, Camera* camera, RenderTarget* scene)
{
    Matrix4 viewProjection = CameraGetWorldToProjection(camera);
    Matrix4 invViewProjection = inverse(viewProjection);
    Matrix4 identity = Matrix4::identity();

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((float*)&identity);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((float*)&identity);

    glBlendFunc(GL_ZERO, GL_ONE);

    glUseProgram(particles->shader_gen);
    {
        UniformBindState bind_state;
        particles->gen_uniforms.bind(bind_state);
        particles->gen_uniforms.bind_uniform(particles->projection_uniform_gen, viewProjection, bind_state);
        particles->gen_uniforms.bind_uniform(particles->inv_projection_uniform_gen, invViewProjection, bind_state);
    }
    RenderUnitQuad();

    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glUseProgram(particles->shader_sim);
    {
        UniformBindState bind_state;
        particles->sim_uniforms.bind(bind_state);
        particles->sim_uniforms.bind_uniform(particles->scene_depth_uniform, RenderTargetTexture(scene, 1), bind_state);
        particles->sim_uniforms.bind_uniform(particles->projection_uniform_sim, viewProjection, bind_state);
        particles->sim_uniforms.bind_uniform(particles->inv_projection_uniform_sim, invViewProjection, bind_state);
    }

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, particles->atomic_count_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles->vertex_buffers[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particles->vertex_buffers[1]);
    ApplyVertexDef(particles->particle_vertex_def);
    glBeginTransformFeedback(GL_POINTS);
    glDrawArraysIndirect(GL_POINTS, 0);
    glEndTransformFeedback();
    ClearVertexDef(particles->particle_vertex_def);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);


    particles->frame_count++;

    // Rotate buffers.

    GLuint tTex = particles->vertex_textures[0];
    GLuint tBuf = particles->vertex_buffers[0];

    for(unsigned int i = 0; i < Particles::BUFFER_COUNT; i++)
    {
        if(i < Particles::BUFFER_COUNT - 1)
        {
            particles->vertex_textures[i] = particles->vertex_textures[i + 1];
            particles->vertex_buffers[i] = particles->vertex_buffers[i + 1];
        }
        else
        {
            particles->vertex_textures[i] = tTex;
            particles->vertex_buffers[i] = tBuf;
        }
    }
}
