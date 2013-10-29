// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "uniforms.h"
#include "gl_all.h"

#include <assert.h>

void UniformElem::init(const char* name, void* source, UniformType ty, GLuint shader)
{
    ptr = source;
    type = ty;
    uniform = glGetUniformLocation(shader, name);
}

void UniformElem::bind(void* _ptr, UniformBindState& bind_state)
{
    switch(type)
    {
    case Uniform_Float:
        glUniform1fv(uniform, 1, (GLfloat*)_ptr);
        break;
    case Uniform_Float2:
        glUniform2fv(uniform, 1, (GLfloat*)_ptr);
        break;
    case Uniform_Float3:
        glUniform3fv(uniform, 1, (GLfloat*)_ptr);
        break;
    case Uniform_Float4:
        glUniform4fv(uniform, 1, (GLfloat*)_ptr);
        break;
    case Uniform_Texture:
        glUniform1i(uniform, bind_state.texture_id);
        glActiveTexture(GL_TEXTURE0 + bind_state.texture_id);
        glBindTexture(GL_TEXTURE_2D, *((GLuint*)_ptr));
        bind_state.texture_id++;
        break;
    case Uniform_Image:
    {
        // TODO: make these customizable.
        GLenum access = GL_READ_WRITE;
        GLenum format = GL_RGBA32I;
        glUniform1i(uniform, bind_state.image_id);
        glBindImageTexture(bind_state.image_id, *((GLuint*)_ptr), 0, GL_FALSE, 0, access, format);
        bind_state.image_id++;
        break;
    }
    default:
        assert(false);
        break;
    }
}

void UniformElem::bind(UniformBindState& bind_state)
{
    if(ptr != NULL)
    {
        bind(ptr, bind_state);
    }
}

Uniforms::Uniforms(unsigned int num) : num_uniforms(num)
{
    uniforms = new UniformElem[num_uniforms];
}

Uniforms::~Uniforms()
{
    delete[] uniforms;
}

void Uniforms::add_uniform(const char* name, void* source, UniformType type, unsigned int i, GLuint shader)
{
    uniforms[i].init(name, source, type, shader);
}

unsigned int Uniforms::get_uniform_idx(const char* name, GLuint shader)
{
    GLuint uniform = glGetUniformLocation(shader, name);

    for(unsigned int i = 0; i < num_uniforms; i++)
    {
        if(uniforms[i].uniform == uniform)
        {
            return i;
        }
    }

    return -1;
}

void Uniforms::bind_uniform(unsigned int idx, void* ptr, UniformBindState& bind_state)
{
    uniforms[idx].bind(ptr, bind_state);
}

void Uniforms::bind(UniformBindState& bind_state)
{
    for(unsigned int i=0; i < num_uniforms; i++)
    {
        uniforms[i].bind(bind_state);
    }
}

void Uniforms::bind()
{
    UniformBindState bind_state;
    bind(bind_state);
}
