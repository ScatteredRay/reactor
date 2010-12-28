// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "simple_mesh.h"
#include <assert.h>
#include <gl.h>

GLuint CreateMesh(unsigned int num_verticies, size_t vertex_size, const void* data)
{
    GLuint vbuffer;
    glGenBuffers(1, &vbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glBufferData(GL_ARRAY_BUFFER, num_verticies * vertex_size, data, GL_STATIC_DRAW);

    return vbuffer;
}

void DestroyMesh(GLuint buffer)
{
    glDeleteBuffers(1, &buffer);
}

VertexDef CreateVertexDef(size_t size, unsigned int attrs)
{
    VertexDef VD = new Vertex_Def();
    VD->stride = size;
    VD->num_attributes = attrs;
    VD->attributes = new VertexAttribute[attrs];

    return VD;
}

void DestroyVertexDef(VertexDef Def)
{
    delete[] Def->attributes;
    delete Def;
}

VertexAttribute* AddVertexAttribute(VertexDef Def,
                                    unsigned int idx,
                                    Vertex_Attributes attr_type,
                                    size_t offset,
                                    unsigned int num_elements,
                                    GLenum data_type)
{
    assert(idx <= Def->num_attributes);
    VertexAttribute* A = &Def->attributes[idx];

    A->attr = attr_type;
    A->offset = offset;
    A->type = data_type;
    A->num_elements = num_elements;

    return A;
}

void ApplyVertexDef(VertexDef Def)
{
    for(unsigned int i=0; i<Def->num_attributes; i++)
    {
        VertexAttribute* A = &Def->attributes[i];
        switch(A->attr)
        {
        case VERTEX_POSITION_ATTR:
            glVertexPointer(A->num_elements,
                            A->type,
                            Def->stride,
                            (void*)A->offset);
            break;
        case VERTEX_NORMAL_ATTR:
            assert(A->num_elements >= 3);
            glNormalPointer(A->type,
                            Def->stride,
                            (void*)A->offset);
            break;
        case VERTEX_COLOR_ATTR:
            glColorPointer(A->num_elements,
                           A->type,
                           Def->stride,
                           (void*)A->offset);
            break;
        case VERTEX_UV_ATTR:
            glTexCoordPointer(A->num_elements,
                              A->type,
                              Def->stride,
                              (void*)A->offset);
            break;
        case VERTEX_OTHER_ATTR:
            // How are we going to set Vertex Attribute indicies, 0 for testing.
            glVertexAttribPointer(0,
                                  A->num_elements,
                                  A->type,
                                  false,
                                  Def->stride,
                                  (void*)A->offset);
            break;
        case VERTEX_NORMALIZED_OTHER_ATTR:
            glVertexAttribPointer(0,
                                  A->num_elements,
                                  A->type,
                                  true,
                                  Def->stride,
                                  (void*)A->offset);
            break;
        }
    }
}
