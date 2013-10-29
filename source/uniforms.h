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
    Uniform_Float4,
    Uniform_Texture,
    Uniform_Image
};

struct UniformBindState
{
    unsigned int texture_id;
    unsigned int image_id;

    UniformBindState() : texture_id(0), image_id(0) {}
};

struct UniformElem
{
    void* ptr;
    UniformType type;
    GLuint uniform;

    void init(const char* name, void* source, UniformType type, GLuint shader);

    void bind(void* _ptr, UniformBindState& bind_state);
    void bind(UniformBindState& bind_state);
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

    unsigned int get_uniform_idx(const char* name, GLuint shader);

    void bind_uniform(unsigned int idx, void* ptr, UniformBindState& bind_state);

    template<typename t>
    void bind_uniform(unsigned int idx, t val, UniformBindState& bind_state)
    {
        bind_uniform(idx, (void*)&val, bind_state);
    }

    void bind(UniformBindState& bind_state);
    void bind();
};

#include "uniforms.inl"

#endif //_UNIFORMS_H_
