// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "particles.h"

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
};

VertexDef GenParticleVertDef()
{
    ParticleVert* proxy = 0;
    VertexDef VD = CreateVertexDef(sizeof(ParticleVert), 1);
    int i = 0;
    AddVertexAttribute(VD, i++, VERTEX_POSITION_ATTR, (size_t)&proxy->location, 4, GL_FLOAT);

    return VD;
}

struct Particles
{
    static const unsigned int BUFFER_COUNT = 2;
    unsigned int vertex_count;
    GLuint vertex_buffers[BUFFER_COUNT];
    GLuint vertex_textures[BUFFER_COUNT];
    GLuint atomic_count_buffer;

    GLuint shader_gen;
    Uniforms gen_uniforms;
    GLuint shader_sim;
    Uniforms sim_uniforms;

    VertexDef particle_vertex_def;

    Particles() : gen_uniforms(3), sim_uniforms(0)
    {
    }

    void CaptureUniforms()
    {
        int i = 0;
        gen_uniforms.add_uniform("particle_buffer", &vertex_textures[0], Uniform_Image, i++, shader_gen);
        // TODO: This only works because `count` is the first element of `GLArraysIndirectCommand`. Make this more general.
        gen_uniforms.add_uniform("particle_count", &atomic_count_buffer, Uniform_Atomic, i++, shader_gen);
        gen_uniforms.add_uniform("max_particles", &vertex_count, i++, shader_gen);
        assert(i == gen_uniforms.num_uniforms);

        i = 0;
        //sim_uniforms.add_uniform("particle_buffer", &vertex_texture, Uniform_Image, i++, shader_sim);
        assert(i == sim_uniforms.num_uniforms);
    }

};

Particles* InitParticles()
{
    Particles* particles = new Particles();

    particles->particle_vertex_def = GenParticleVertDef();

    particles->shader_gen = CreateShaderProgram(SHADER_PARTICLE_GEN);
    particles->shader_sim = CreateShaderProgram(SHADER_PARTICLE_SIM);
    BindShaderToUnitQuad(particles->shader_gen);
    const char* varyings[] = {"out_Position"};
    glTransformFeedbackVaryings(particles->shader_sim, 1, varyings, GL_INTERLEAVED_ATTRIBS);
    VertexDefBindToShader(particles->particle_vertex_def, particles->shader_sim);
    particles->CaptureUniforms();

    particles->vertex_count = 65536;
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

void UpdateParticles(Particles* particles)
{
    Matrix4 identity = Matrix4::identity();

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((float*)&identity);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((float*)&identity);

    glBlendFunc(GL_ZERO, GL_ONE);

    glUseProgram(particles->shader_gen);
    particles->gen_uniforms.bind();
    RenderUnitQuad();

    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);

    glBlendFunc(GL_ONE, GL_ZERO);

    glUseProgram(particles->shader_sim);
    particles->sim_uniforms.bind();

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
