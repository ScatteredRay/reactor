// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "particles.h"

#include "simple_texture.h"
#include "simple_shader.h"
#include "render_target.h"
#include "render_util.h"
#include "uniforms.h"
#include "reporting.h"

#include "gl_all.h"
#include <assert.h>

struct Particles
{
    unsigned int vertex_count;
    GLuint vertex_buffer;
    GLuint vertex_texture;
    GLuint atomic_count_buffer;

    GLuint shader_gen;
    Uniforms gen_uniforms;
    GLuint shader_sim;
    Uniforms sim_uniforms;

    Particles() : gen_uniforms(3), sim_uniforms(0)
    {
    }

    void CaptureUniforms()
    {
        int i = 0;
        gen_uniforms.add_uniform("particle_buffer", &vertex_texture, Uniform_Image, i++, shader_gen);
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

    particles->shader_gen = CreateShaderProgram(SHADER_PARTICLE_GEN);
    particles->shader_sim = CreateShaderProgram(SHADER_PARTICLE_SIM);
    BindShaderToUnitQuad(particles->shader_gen);
    BindShaderToUnitQuad(particles->shader_sim);
    particles->CaptureUniforms();

    particles->vertex_count = 65536;
    unsigned int vertex_len = 4 * 4;
    glGenBuffers(1, &particles->vertex_buffer);
    glBindBuffer(GL_TEXTURE_BUFFER, particles->vertex_buffer);
    glBufferData(GL_TEXTURE_BUFFER, vertex_len * particles->vertex_count, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);

    glGenTextures(1, &particles->vertex_texture);
    glBindTexture(GL_TEXTURE_BUFFER, particles->vertex_texture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, particles->vertex_buffer);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);

    glGenBuffers(1, &particles->atomic_count_buffer);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, particles->atomic_count_buffer);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
    // Do we need to call BufferData if we just map it right after?
    GLuint* atomic_data = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_WRITE_BIT);
    *atomic_data = 0;
    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, NULL);

    return particles;
}

void DestroyParticles(Particles* particles)
{
    DestroyTexture(particles->vertex_buffer);
    DestroyProgramAndAttachedShaders(particles->shader_gen);
    DestroyProgramAndAttachedShaders(particles->shader_sim);
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

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    //glBlendFunc(GL_ZERO, GL_ONE);

    glUseProgram(particles->shader_sim);
    particles->sim_uniforms.bind();
    RenderUnitQuad();

    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    glBlendFunc(GL_ONE, GL_ZERO);
}
