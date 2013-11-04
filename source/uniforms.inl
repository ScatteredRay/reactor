// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "vectormath.h"
#include <assert.h>

template<typename T>
struct MetaUniform
{
    static T* ptr(T* p)
    {
        return p;
    }

    static UniformType type()
    {
        assert(false);
        return Uniform_Unknown;
    }
};

template<>
struct MetaUniform<float>
{
    static float* ptr(float* p)
    {
        return p;
    }

    static UniformType type()
    {
        return Uniform_Float;
    }
};

template<>
struct MetaUniform<Vector3>
{
    static float* ptr(Vector3* p)
    {
        return &((*p)[0]);
    }

    static UniformType type()
    {
        return Uniform_Float3;
    }
};

template<>
struct MetaUniform<Vector4>
{
    static float* ptr(Vector4* p)
    {
        return &((*p)[0]);
    }

    static UniformType type()
    {
        return Uniform_Float4;
    }
};

template<>
struct MetaUniform<int>
{
    static int* ptr(int* p)
    {
        return p;
    }

    static UniformType type()
    {
        return Uniform_Int;
    }
};

template<>
struct MetaUniform<unsigned int>
{
    static unsigned int* ptr(unsigned int* p)
    {
        return p;
    }

    static UniformType type()
    {
        return Uniform_UInt;
    }
};

template<typename T>
void Uniforms::add_uniform(const char* name, T* source, unsigned int i, GLuint shader)
{
    add_uniform(name,
                MetaUniform<T>::ptr(source),
                MetaUniform<T>::type(),
                i, shader);
}
