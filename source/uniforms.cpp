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

void UniformElem::bind()
{
    switch(type)
    {
    case Uniform_Float:
        glUniform1fv(uniform, 1, (GLfloat*)ptr);
        break;
    case Uniform_Float2:
        glUniform2fv(uniform, 1, (GLfloat*)ptr);
        break;
    case Uniform_Float3:
        glUniform3fv(uniform, 1, (GLfloat*)ptr);
        break;
    case Uniform_Float4:
        glUniform4fv(uniform, 1, (GLfloat*)ptr);
        break;
    default:
        assert(false);
        break;
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

void Uniforms::bind()
{
    for(unsigned int i=0; i < num_uniforms; i++)
    {
        uniforms[i].bind();
    }
}
