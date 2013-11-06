// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include "simple_shader.h"
#include "reporting.h"
#include <assert.h>
#include <cstdio>
#include "gl_all.h"

char* ReadShaderSourceFromFile(const char* filename)
{
    FILE* file = fopen(filename, "r");

    if(!file)
        return NULL;

    fseek(file, 0, SEEK_END);
    size_t len = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* ret = (char*)malloc(len+1);
    size_t readc = fread(ret, 1, len, file);
    // This seems to come out smaller on Win32.
    assert(readc <= len);
    ret[readc] = '\0';

    fclose(file);

    return ret;
}

GLuint CreateShader(const char* filename, GLenum type)
{
    GLuint shader = glCreateShader(type);
    assert(shader);

    char* src[1];
    src[0] = ReadShaderSourceFromFile(filename);
    int len[1];
    len[0] = -1;

    glShaderSource(shader, 1, (const char**)src, len);
    free(src[0]);

    glCompileShader(shader);

    GLint link_status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &link_status);

    if(link_status == GL_FALSE)
    {
        size_t buf_len;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, (GLint*)&buf_len);
        char* buffer = (char*)malloc(buf_len);
        size_t read_len;
        glGetShaderInfoLog(shader, buf_len, (GLsizei*)&read_len, buffer);
        assert(buf_len == read_len + 1);
        log(LOG_ERROR, buffer);
        free(buffer);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

const char* GetVShaderFilename(shader_id shader);
const char* GetGShaderFilename(shader_id shader);
const char* GetPShaderFilename(shader_id shader);

GLuint CreateShaderProgram(shader_id shader)
{
    GLuint vshader, gshader, pshader, program;
    vshader = CreateShader(GetVShaderFilename(shader), GL_VERTEX_SHADER);
    if(!vshader) goto error;

    pshader = CreateShader(GetPShaderFilename(shader), GL_FRAGMENT_SHADER);
    if(!pshader) goto error_pshader;

    const char* gshader_file = GetGShaderFilename(shader);
    if(gshader_file)
    {
        gshader = CreateShader(gshader_file, GL_GEOMETRY_SHADER);
        if(!gshader) goto error_gshader;
    }
    else
    {
        gshader = NULL;
    }

    program = glCreateProgram();
    assert(program);

    glAttachShader(program, vshader);
    glAttachShader(program, pshader);
    if(gshader)
        glAttachShader(program, gshader);

    glProgramParameteri(program, GL_GEOMETRY_INPUT_TYPE, GL_POINTS);
    glProgramParameteri(program, GL_GEOMETRY_OUTPUT_TYPE, GL_POINTS);
    glProgramParameteri(program, GL_GEOMETRY_VERTICES_OUT, 1);

    glLinkProgram(program);

    GLint link_status;

    glGetProgramiv(program, GL_LINK_STATUS, &link_status);

     if(link_status == GL_FALSE)
     {
         size_t buf_len = 0;
         glGetProgramiv(shader, GL_INFO_LOG_LENGTH, (GLsizei*)&buf_len);
         if(buf_len)
         {
             char* buffer = (char*)malloc(buf_len);
             assert(buffer);
             size_t read_len = 0;
             glGetProgramInfoLog(shader, buf_len, (GLsizei*)&read_len, buffer);
             assert(buf_len == read_len + 1);
             buffer[buf_len] = '\0';
             log(LOG_ERROR, buffer);
             free(buffer);
         }
         else
         {
             log(LOG_ERROR, "Unknown error linking program");
         }
         goto error_program;
     }

     glDeleteShader(pshader);
     glDeleteShader(vshader);
     if(gshader)
         glDeleteShader(gshader);

     return program;

error_program:
     glDetachShader(program, pshader);
     glDetachShader(program, vshader);
     if(gshader)
         glDetachShader(program, gshader);
     glDeleteProgram(program);
     if(gshader)
         glDeleteShader(gshader);
error_gshader:
     glDeleteShader(pshader);
error_pshader:
     glDeleteShader(vshader);
error:
     return 0;
}

void DestroyProgramAndAttachedShaders(GLuint program)
{
    glDeleteProgram(program);
}

const char* ShaderFileMap[][3] =

{
    { "standard.vsh", NULL, "constant_color.psh" },
    { "environment.vsh",  NULL, "environment.psh" },
    { "character.vsh", NULL, "character.psh" },
    { "alignedquad.vsh", NULL, "atmospherics.psh" },
    { "alignedquad.vsh", NULL, "particle_gen.psh" },
    { "particle_sim.vsh", "particle_sim.gsh", "particle_sim.psh" }
};

const char* GetVShaderFilename(shader_id shader)
{
    return ShaderFileMap[shader][0];
}

const char* GetGShaderFilename(shader_id shader)
{
    return ShaderFileMap[shader][1];
}

const char* GetPShaderFilename(shader_id shader)
{
    return ShaderFileMap[shader][2];
}
