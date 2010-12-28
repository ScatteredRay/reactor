// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _SIMPLE_MESH_H_
#define _SIMPLE_MESH_H_

#include <cstring> // size_t

typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLsizei;

enum Vertex_Attributes
{
    VERTEX_POSITION_ATTR,
    VERTEX_NORMAL_ATTR,
    VERTEX_COLOR_ATTR,
    VERTEX_UV_ATTR,
    VERTEX_OTHER_ATTR,
    VERTEX_NORMALIZED_OTHER_ATTR
};

GLuint CreateMesh(unsigned int num_verticies, size_t vertex_size, const void* data);
void DestroyMesh(GLuint buffer);

struct VertexAttribute
{
    Vertex_Attributes attr;
    size_t offset;
    GLenum type;
    unsigned int num_elements;
};

struct Vertex_Def
{
    size_t stride;
    unsigned int num_attributes;
    VertexAttribute* attributes;
};

typedef Vertex_Def* VertexDef;

VertexDef CreateVertexDef(size_t size, unsigned int attrs);
VertexAttribute* AddVertexAttribute(VertexDef Def,
                                    unsigned int idx,
                                    Vertex_Attributes attr_type,
                                    size_t offset,
                                    unsigned int num_elements,
                                    GLenum data_type);
void ApplyVertexDef(VertexDef Def);

void DestroyVertexDef(VertexDef Def);

#endif //_SIMPLE_MESH_H_
