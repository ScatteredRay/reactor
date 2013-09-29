// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include "simple_shader.h"
#include "camera.h"

#include "gl_all.h"
#include <assert.h>

struct DeferredRender
{
    GLuint postprocess_shader;
    GLuint color_buffer_uniform;
    GLuint depth_buffer_uniform;
};

// TODO, make this dynamic
const unsigned int ENV_NUM_FRAMEBUFFERS = 2;

struct RenderTarget
{
    GLuint framebuffer;
    GLuint depth_render_buffer;
    GLuint framebuffer_textures[ENV_NUM_FRAMEBUFFERS];
    GLenum render_buffer_list[ENV_NUM_FRAMEBUFFERS];
};

void InitRenderTarget(RenderTarget* t, unsigned int width, unsigned int height)
{
    glGenFramebuffers(1, &t->framebuffer);
    glGenTextures(ENV_NUM_FRAMEBUFFERS, t->framebuffer_textures);

    glBindFramebuffer(GL_FRAMEBUFFER, t->framebuffer);


    glGenRenderbuffers(1, &t->depth_render_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, t->depth_render_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, t->depth_render_buffer);

    for(unsigned int i=0; i<ENV_NUM_FRAMEBUFFERS; i++)
    {
        glBindTexture(GL_TEXTURE_2D, t->framebuffer_textures[i]);

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
                               t->framebuffer_textures[i],
                               0);

        t->render_buffer_list[i] = GL_COLOR_ATTACHMENT0 + i;
    }

    glDrawBuffers(ENV_NUM_FRAMEBUFFERS, t->render_buffer_list);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(status == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget* InitRenderTarget(unsigned int width, unsigned int height)
{
    RenderTarget* t = new RenderTarget();

    InitRenderTarget(t, width, height);

    return t;
}

void ReleaseRenderTarget(RenderTarget* t)
{
    glDeleteRenderbuffers(1, &t->depth_render_buffer);
    glDeleteTextures(ENV_NUM_FRAMEBUFFERS, t->framebuffer_textures);
    glDeleteFramebuffers(1, &t->framebuffer);
}

void DestroyRenderTarget(RenderTarget* t)
{
    ReleaseRenderTarget(t);
    delete t;
}

void ResizeRenderTarget(RenderTarget* t, unsigned int width, unsigned int height)
{
    ReleaseRenderTarget(t);
    InitRenderTarget(t, width, height);
}

void BindRenderTarget(RenderTarget* t)
{
    glBindFramebuffer(GL_FRAMEBUFFER, t->framebuffer);
}

void UnbindRenderTarget(RenderTarget* t)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

DeferredRender* InitDeferred()
{
    DeferredRender* r = new DeferredRender();

    r->postprocess_shader = CreateShaderProgram(SHADER_ATMOSPHERICS);
    r->color_buffer_uniform = glGetUniformLocation(r->postprocess_shader, "color_buffer");
    r->depth_buffer_uniform = glGetUniformLocation(r->postprocess_shader, "depth_buffer");
    return r;
}

void DestroyDeferred(DeferredRender* r)
{
    DestroyProgramAndAttachedShaders(r->postprocess_shader);
    delete r;
}

void RenderUnitQuad();

void RenderDeferred(DeferredRender* r, RenderTarget* t)
{
    glUseProgram(r->postprocess_shader);

    //Clean me up!

    glUniform1i(r->color_buffer_uniform, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, t->framebuffer_textures[0]);

    glUniform1i(r->depth_buffer_uniform, 1);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, t->framebuffer_textures[1]);

    RenderUnitQuad();

    glBindTexture(GL_TEXTURE_2D, 0);
}
