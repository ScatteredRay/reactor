// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include "simple_shader.h"
#include "reporting.h"
#include <assert.h>
#include <gl.h>
#include <cstdio>

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
    assert(readc == len);
    ret[len] = '\0';

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
        buffer[buf_len] = '\0';
        log_own_buffer(LOG_ERR, buffer);
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

const char* GetVShaderFilename(shader_id shader);
const char* GetPShaderFilename(shader_id shader);

GLuint CreateShaderProgram(shader_id shader)
{
    GLuint vshader, pshader, program;
    vshader = CreateShader(GetVShaderFilename(shader), GL_VERTEX_SHADER);
    if(!vshader) goto error;

    pshader = CreateShader(GetPShaderFilename(shader), GL_FRAGMENT_SHADER);
    if(!pshader) goto error_pshader;

    program = glCreateProgram();
    assert(program);

    glAttachShader(program, vshader);
    glAttachShader(program, pshader);

    glLinkProgram(program);

    GLint link_status;

    glGetProgramiv(program, GL_LINK_STATUS, &link_status);

     if(link_status == GL_FALSE)
    {
        size_t buf_len;
        glGetProgramiv(shader, GL_INFO_LOG_LENGTH, (GLsizei*)&buf_len);
        char* buffer = (char*)malloc(buf_len);
        size_t read_len;
        glGetProgramInfoLog(shader, buf_len, (GLsizei*)&read_len, buffer);
        assert(buf_len == read_len + 1);
        buffer[buf_len] = '\0';
        log_own_buffer(LOG_ERR, buffer);
        goto error_program;
    }

    return program;

error_program:
    glDetachShader(program, pshader);
    glDetachShader(program, vshader);
    glDeleteProgram(program);
    glDeleteShader(pshader);
error_pshader:
    glDeleteShader(vshader);
error:
    return 0;
}

void DestroyProgramAndAttachedShaders(GLuint program)
{

    const int MAX_ATTACHED_SHADERS = 2;

    GLsizei attached_shaders;
    GLuint shaders[MAX_ATTACHED_SHADERS];

    glGetAttachedShaders(program, MAX_ATTACHED_SHADERS, &attached_shaders, shaders);
    
    for(int i=0; i<attached_shaders; i++)
    {
        glDetachShader(program, shaders[i]);
        glDeleteShader(shaders[i]);
    }

    glDeleteProgram(program);
}

const char* ShaderFileMap[][2] =

{    { "standard.vsh", "constant_color.psh" },
     { "character.vsh", "character.psh" }
};

const char* GetVShaderFilename(shader_id shader)
{
    return ShaderFileMap[shader][0];
}

const char* GetPShaderFilename(shader_id shader)
{
    return ShaderFileMap[shader][1];
}
