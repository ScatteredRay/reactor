// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _RENDER_TARGET_H_
#define _RENDER_TARGET_H_

typedef unsigned int GLuint;

struct RenderTarget;
RenderTarget* InitRenderTarget(unsigned int width, unsigned int height, unsigned int num_framebuffers);
void DestroyRenderTarget(RenderTarget* t);
void ResizeRenderTarget(RenderTarget* t, unsigned int width, unsigned int height);
void BindRenderTarget(RenderTarget* t);
void UnbindRenderTarget(RenderTarget* t);

// Returns: borrowed texture.
GLuint RenderTargetTexture(RenderTarget* t, unsigned int index = 0);


#endif //_RENDER_TARGET_H_
