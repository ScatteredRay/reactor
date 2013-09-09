// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "simple_mesh.h"
#include <assert.h>
#include "gl_all.h"

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
    for(int i = 0; i< Def->num_attributes; i++)
    {
        delete[] Def->attributes[i].attr_name;
    }
    delete[] Def->attributes;
    delete Def;
}

VertexAttribute* AddVertexAttribute(VertexDef Def,
                                    unsigned int idx,
                                    Vertex_Attributes attr_type,
                                    size_t offset,
                                    unsigned int num_elements,
                                    GLenum data_type,
                                    const char* attr_name)
{
    assert(idx <= Def->num_attributes);
    VertexAttribute* A = &Def->attributes[idx];

    A->attr = attr_type;
    A->idx = idx;
    A->offset = offset;
    A->type = data_type;
    A->num_elements = num_elements;
    if(attr_name)
    {
        A->attr_name = new char[strlen(attr_name) + 1];
        strcpy(A->attr_name, attr_name);
    }
    else
    {
        A->attr_name = 0;
    }

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
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(A->num_elements,
                            A->type,
                            Def->stride,
                            (void*)A->offset);
            break;
        case VERTEX_NORMAL_ATTR:
            glEnableClientState(GL_NORMAL_ARRAY);
            assert(A->num_elements >= 3);
            glNormalPointer(A->type,
                            Def->stride,
                            (void*)A->offset);
            break;
        case VERTEX_COLOR_ATTR:
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(A->num_elements,
                           A->type,
                           Def->stride,
                           (void*)A->offset);
            break;
        case VERTEX_UV_ATTR:
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(A->num_elements,
                              A->type,
                              Def->stride,
                              (void*)A->offset);
            break;
        case VERTEX_OTHER_ATTR:
        case VERTEX_OTHER_ATTR_0:
        case VERTEX_OTHER_ATTR_1:
        case VERTEX_OTHER_ATTR_2:
        case VERTEX_OTHER_ATTR_3:
            {
                int idx = A->idx;
                if(A->attr > VERTEX_OTHER_ATTR)
                {
                    idx = A->attr - VERTEX_OTHER_ATTR_0 + 1;
                    assert(idx < 4);
                }
                glEnableVertexAttribArray(idx);
                glVertexAttribPointer(idx,
                                      A->num_elements,
                                      A->type,
                                      false,
                                      Def->stride,
                                      (void*)A->offset);
            }
            break;
        case VERTEX_NORMALIZED_OTHER_ATTR:
        case VERTEX_NORMALIZED_OTHER_ATTR_0:
        case VERTEX_NORMALIZED_OTHER_ATTR_1:
        case VERTEX_NORMALIZED_OTHER_ATTR_2:
        case VERTEX_NORMALIZED_OTHER_ATTR_3:
            {
                int idx = A->idx;
                if(A->attr > VERTEX_NORMALIZED_OTHER_ATTR)
                {
                    idx = A->attr - VERTEX_NORMALIZED_OTHER_ATTR_0 + 1;
                    assert(idx < 4);
                }
                glEnableVertexAttribArray(idx);
                glVertexAttribPointer(idx,
                                      A->num_elements,
                                      A->type,
                                      true,
                                      Def->stride,
                                      (void*)A->offset);
            }
            break;
        }
    }
}

void ClearVertexDef(VertexDef Def)
{
    for(unsigned int i=0; i<Def->num_attributes; i++)
    {
        VertexAttribute* A = &Def->attributes[i];
        switch(A->attr)
        {
        case VERTEX_POSITION_ATTR:
            glDisableClientState(GL_VERTEX_ARRAY);
            break;
        case VERTEX_NORMAL_ATTR:
            glDisableClientState(GL_NORMAL_ARRAY);
            break;
        case VERTEX_COLOR_ATTR:
            glDisableClientState(GL_COLOR_ARRAY);
            break;
        case VERTEX_UV_ATTR:
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            break;
        case VERTEX_OTHER_ATTR:
        case VERTEX_OTHER_ATTR_0:
        case VERTEX_OTHER_ATTR_1:
        case VERTEX_OTHER_ATTR_2:
        case VERTEX_OTHER_ATTR_3:
            {
                int idx = A->idx;
                if(A->attr > VERTEX_OTHER_ATTR)
                {
                    idx = A->attr - VERTEX_OTHER_ATTR_0 + 1;
                    assert(idx < 4);
                }
                glDisableVertexAttribArray(idx);
            }
            break;
        case VERTEX_NORMALIZED_OTHER_ATTR:
        case VERTEX_NORMALIZED_OTHER_ATTR_0:
        case VERTEX_NORMALIZED_OTHER_ATTR_1:
        case VERTEX_NORMALIZED_OTHER_ATTR_2:
        case VERTEX_NORMALIZED_OTHER_ATTR_3:
            {
                int idx = A->idx;
                if(A->attr > VERTEX_NORMALIZED_OTHER_ATTR)
                {
                    idx = A->attr - VERTEX_NORMALIZED_OTHER_ATTR_0 + 1;
                    assert(idx < 4);
                }
                glDisableVertexAttribArray(idx);
            }
            break;
        }
    }
}

void VertexDefBindToShader(VertexDef Def, GLuint shader)
{
    for(unsigned int i=0; i<Def->num_attributes; i++)
    {
        VertexAttribute* A = &Def->attributes[i];
        switch(A->attr)
        {
        case VERTEX_OTHER_ATTR:
        case VERTEX_OTHER_ATTR_0:
        case VERTEX_OTHER_ATTR_1:
        case VERTEX_OTHER_ATTR_2:
        case VERTEX_OTHER_ATTR_3:
        case VERTEX_NORMALIZED_OTHER_ATTR:
        case VERTEX_NORMALIZED_OTHER_ATTR_0:
        case VERTEX_NORMALIZED_OTHER_ATTR_1:
        case VERTEX_NORMALIZED_OTHER_ATTR_2:
        case VERTEX_NORMALIZED_OTHER_ATTR_3:
            if(A->attr_name)
                glBindAttribLocation(shader, A->idx, A->attr_name);
            break;
        default:
            break;
        }
    }
    glLinkProgram(shader);
}
