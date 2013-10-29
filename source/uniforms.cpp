// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "uniforms.h"
#include "gl_all.h"

#include <assert.h>

struct UniformElem
{
    void* ptr;
    UniformType type;
    GLuint uniform;

    void init(const char* name, void* source, UniformType type, GLuint shader);

    void bind(unsigned int& texture_id, unsigned int& image_id);
};

void UniformElem::init(const char* name, void* source, UniformType ty, GLuint shader)
{
    ptr = source;
    type = ty;
    uniform = glGetUniformLocation(shader, name);
}

void UniformElem::bind(unsigned int& texture_id, unsigned int& image_id)
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
    case Uniform_Texture:
        glUniform1i(uniform, texture_id);
        glActiveTexture(GL_TEXTURE0 + texture_id);
        glBindTexture(GL_TEXTURE_2D, *((GLuint*)ptr));
        texture_id++;
        break;
    case Uniform_Image:
    {
        // TODO: make these customizable.
        GLenum access = GL_READ_WRITE;
        GLenum format = GL_RGBA32I;
        glUniform1i(uniform, image_id);
        glBindImageTexture(image_id, *((GLuint*)ptr), 0, GL_FALSE, 0, access, format);
        image_id++;
        break;
    }
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
    unsigned int texture_id = 0;
    unsigned int image_id = 0;
    for(unsigned int i=0; i < num_uniforms; i++)
    {
        uniforms[i].bind(texture_id, image_id);
    }
}
