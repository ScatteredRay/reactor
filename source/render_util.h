// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _RENDER_UTIL_H_
#define _RENDER_UTIL_H_

typedef unsigned int GLuint;

void InitRenderUtil();
void CleanupRenderUtil();
void RenderUnitQuad();
void BindShaderToUnitQuad(GLuint shader);

#endif //_RENDER_UTIL_H_
