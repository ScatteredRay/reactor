// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _SIMPLE_SHADER_H_
#define _SIMPLE_SHADER_H_

typedef unsigned int GLuint;

// Map declared near the bottom of simple_shader.cpp needs to be kept in sync!
enum shader_id
{
    SHADER_CONSTANT_COLOR = 0,
    SHADER_ENVIRONMENT = 1,
    SHADER_CHARACTER = 2,
    SHADER_ATMOSPHERICS = 3,
    SHADER_PARTICLE_GEN = 4,
    SHADER_PARTICLE_SIM = 5
};

GLuint CreateShaderProgram(shader_id shader);
void DestroyProgramAndAttachedShaders(GLuint program);

#endif //_SIMPLE_SHADER_H_
