// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "render_util.h"

#include "simple_mesh.h"
#include "simple_vectors.h"

#include "gl_all.h"

GLuint gQuad;
VertexDef gQuadVerts;

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
 {{1.0f, -1.0f, 0.0f},  {1.0f, 0.0f}},
 {{-1.0f, 1.0f, 0.0f},  {0.0f, 1.0f}},
 {{-1.0f, 1.0f, 0.0f},  {0.0f, 1.0f}},
 {{1.0f, -1.0f, 0.0f},  {1.0f, 0.0f}},
 {{1.0f, 1.0f, 0.0f},   {1.0f, 1.0f}}};

void InitRenderUtil()
{
    gQuadVerts = gen_tex_quad_vert_def();
    gQuad = CreateMesh(6, sizeof(tex_quad_vert), quad_mesh);
}

void CleanupRenderUtil()
{
    DestroyMesh(gQuad);
    DestroyVertexDef(gQuadVerts);
}

void BindShaderToUnitQuad(GLuint shader)
{
    VertexDefBindToShader(gQuadVerts, shader);
}

void RenderUnitQuad()
{
    glBindBuffer(GL_ARRAY_BUFFER, gQuad);
    ApplyVertexDef(gQuadVerts);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    ClearVertexDef(gQuadVerts);
}
