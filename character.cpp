#include <character.h>
#include <gl.h>
#include "simple_mesh.h"
#include "simple_vectors.h"
#include "simple_shader.h"
#include "simple_texture.h"

struct Character
{
};

VertexDef character_vert_def;
GLuint character_mesh;
GLuint character_texture;
GLint character_sampler_uniform;
GLuint character_shader;

struct character_vert
{
    vector3 location;
    vector2 uv;
};

VertexDef gen_character_vert_def()
{
    character_vert* proxy = 0;
    VertexDef VD = CreateVertexDef(sizeof(character_vert), 2);
    int i = 0;
    AddVertexAttribute(VD, i++, VERTEX_POSITION_ATTR, (size_t)&proxy->location, 3, GL_FLOAT);
    AddVertexAttribute(VD, i++, VERTEX_UV_ATTR, (size_t)&proxy->uv, 2, GL_FLOAT);

    return VD;
}

const character_vert char_mesh[] =
{{{-0.1f, -0.1f, 0.0f}, {0.0f, 0.0f}},
 {{-0.1f, 0.1f, 0.0f},  {0.0f, 1.0f}},
 {{0.1f, -0.1f, 0.0f},  {1.0f, 0.0f}},
 {{-0.1f, 0.1f, 0.0f},  {0.0f, 1.0f}},
 {{0.1f, -0.1f, 0.0f},  {1.0f, 0.0f}},
 {{0.1f, 0.1f, 0.0f},   {1.0f, 1.0f}}};

void InitCharacters()
{
    character_vert_def = gen_character_vert_def();

    character_mesh = CreateMesh(6, sizeof(character_vert), char_mesh);

    character_shader = CreateShaderProgram(SHADER_CHARACTER);
    character_sampler_uniform = glGetUniformLocation(character_shader, "sprite_tex");

    character_texture = CreateTextureFromBMP("data/PFX_test_001.bmp");
}

void FinishCharacters()
{
    DestroyTexture(character_texture);
    DestroyProgramAndAttachedShaders(character_shader);
    DestroyMesh(character_mesh);
}

Character* CreateCharacter()
{
    Character* character = new Character();
    return character;
}

void RenderCharacter(Character* character)
{
    glUseProgram(character_shader);
    glBindBuffer(GL_ARRAY_BUFFER, character_mesh);
    ApplyVertexDef(character_vert_def);

    glUniform1i(character_sampler_uniform, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, character_texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DestroyCharacter(Character* character)
{
    delete character;
}
