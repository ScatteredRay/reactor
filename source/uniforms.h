// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _UNIFORMS_H_
#define _UNIFORMS_H_

typedef unsigned int GLuint;

enum UniformType {
    Uniform_Unknown,
    Uniform_Float,
    Uniform_Float2,
    Uniform_Float3,
    Uniform_Float4
};

struct UniformElem
{
    void* ptr;
    UniformType type;
    GLuint uniform;

    void init(const char* name, void* source, UniformType type, GLuint shader);

    void bind();
};

struct Uniforms
{
    unsigned int num_uniforms;
    UniformElem* uniforms;

    Uniforms(unsigned int num);
    ~Uniforms();

    void add_uniform(const char* name, void* source, UniformType type, unsigned int i, GLuint shader);

    template<typename T>
    void add_uniform(const char* name, T* source, unsigned int i, GLuint shader);

    void bind();
};

#include "uniforms.inl"

#endif //_UNIFORMS_H_
