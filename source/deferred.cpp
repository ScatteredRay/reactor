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
    GLuint color_buffer_uniform;
    GLuint depth_buffer_uniform;
};

DeferredRender* InitDeferred(Environment* env)
{
    DeferredRender* r = new DeferredRender();

    r->postprocess_shader = CreateShaderProgram(SHADER_ATMOSPHERICS);
    r->color_buffer_uniform = glGetUniformLocation(r->postprocess_shader, "color_buffer");
    r->depth_buffer_uniform = glGetUniformLocation(r->postprocess_shader, "depth_buffer");

    CaptureScatteringUniforms(env, r->postprocess_shader);

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

    //Clean me up!

    glUniform1i(r->color_buffer_uniform, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, RenderTargetTexture(t, 0));

    glUniform1i(r->depth_buffer_uniform, 1);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, RenderTargetTexture(t, 1));

    GetScatteringUniforms(env)->bind();

    RenderUnitQuad();

    glBindTexture(GL_TEXTURE_2D, 0);
}
