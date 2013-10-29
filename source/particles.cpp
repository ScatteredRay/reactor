// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "particles.h"

#include "simple_texture.h"
#include "simple_shader.h"
#include "render_target.h"
#include "render_util.h"
#include "uniforms.h"

#include "gl_all.h"
#include <assert.h>

struct Particles
{
    unsigned int vertex_size_sqrt;
    GLuint vertex_buffer;

    GLuint shader_gen;
    Uniforms gen_uniforms;
    GLuint shader_sim;
    Uniforms sim_uniforms;

    Particles() : gen_uniforms(1), sim_uniforms(1)
    {
    }

    void CaptureUniforms()
    {
        int i = 0;
        gen_uniforms.add_uniform("particle_buffer", &vertex_buffer, Uniform_Image, i++, shader_gen);
        assert(i == gen_uniforms.num_uniforms);

        i = 0;
        sim_uniforms.add_uniform("particle_buffer", &vertex_buffer, Uniform_Image, i++, shader_sim);
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

    particles->vertex_size_sqrt = 512;
    particles->vertex_buffer =
        CreateTexture(
            particles->vertex_size_sqrt,
            particles->vertex_size_sqrt,
            NULL,
            GL_RGBA32I,
            GL_INT);

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
    glUseProgram(particles->shader_gen);
    particles->gen_uniforms.bind();
    RenderUnitQuad();

    glUseProgram(particles->shader_sim);
    particles->sim_uniforms.bind();
    RenderUnitQuad();
}
