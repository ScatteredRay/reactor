// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "deferred.h"

#include "simple_shader.h"
#include "camera.h"
#include "uniforms.h"
#include "environment.h"
#include "render_target.h"
#include "render_util.h"

#include "gl_all.h"

struct DeferredRender
{
    GLuint postprocess_shader;
    unsigned int color_buffer_uniform;
    unsigned int depth_buffer_uniform;
};

DeferredRender* InitDeferred(Environment* env)
{
    DeferredRender* r = new DeferredRender();

    r->postprocess_shader = CreateShaderProgram(SHADER_ATMOSPHERICS);
    CaptureScatteringUniforms(env, r->postprocess_shader);

    Uniforms* uniforms = GetScatteringUniforms(env);
    r->color_buffer_uniform = uniforms->get_uniform_idx("color_buffer", r->postprocess_shader);
    r->depth_buffer_uniform = uniforms->get_uniform_idx("depth_buffer", r->postprocess_shader);

    return r;
}

void DestroyDeferred(DeferredRender* r)
{
    DestroyProgramAndAttachedShaders(r->postprocess_shader);
    delete r;
}

void RenderDeferred(DeferredRender* r, RenderTarget* t, Environment* env)
{
    glUseProgram(r->postprocess_shader);

    UniformBindState bind_state;
    Uniforms* uniforms = GetScatteringUniforms(env);
    uniforms->bind(bind_state);
    uniforms->bind_uniform(r->color_buffer_uniform, RenderTargetTexture(t, 0), bind_state);
    uniforms->bind_uniform(r->depth_buffer_uniform, RenderTargetTexture(t, 1), bind_state);    

    RenderUnitQuad();

    glBindTexture(GL_TEXTURE_2D, 0);
}
