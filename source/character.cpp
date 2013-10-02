#include "character.h"
#include "gl_all.h"
#include "vectormath.h"
#include "persist.h"

#include "simple_mesh.h"
#include "simple_vectors.h"
#include "simple_shader.h"
#include "simple_texture.h"
#include "input.h"

struct Character
{
    float speed;
    Vector3 location;
    PlayerInput* input;
};

template <>
struct Reflect_Type<Character>
{
    static void metadata(Reflect& reflect)
    {
        reflect(&Character::speed, "Speed");
    }
};

VertexDef character_vert_def;
GLuint character_mesh;
GLuint character_texture;
GLint character_sampler_uniform;
GLint character_local_to_world_uniform;
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
{{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
 {{1.0f, -1.0f, 0.0f},  {1.0f, 0.0f}},
 {{-1.0f, 1.0f, 0.0f},  {0.0f, 1.0f}},
 {{-1.0f, 1.0f, 0.0f},  {0.0f, 1.0f}},
 {{1.0f, -1.0f, 0.0f},  {1.0f, 0.0f}},
 {{1.0f, 1.0f, 0.0f},   {1.0f, 1.0f}}};

void InitCharacters()
{
    character_vert_def = gen_character_vert_def();

    character_mesh = CreateMesh(6, sizeof(character_vert), char_mesh);

    character_shader = CreateShaderProgram(SHADER_CHARACTER);
    character_sampler_uniform = glGetUniformLocation(character_shader, "sprite_tex");
    character_local_to_world_uniform = glGetUniformLocation(character_shader, "local_to_world");

    character_texture = CreateTextureFromBMP("data/char.bmp");
}

void FinishCharacters()
{
    DestroyTexture(character_texture);
    DestroyProgramAndAttachedShaders(character_shader);
    DestroyMesh(character_mesh);
}

Character* CreateCharacter(PlayerInput* input)
{
    Character* character = persist_create_from_config<Character>("data/character.json");

    character->location = Vector3(0.0f, 0.0f, 0.0f);
    character->input = input;

    return character;
}

float SmoothInput(float x)
{
    return 10 * x * x * (x >= 0 ? 1 : -1);
}

void UpdateCharacter(Character* character, float DeltaTime)
{
    float player_speed = 0.8f;

    character->location
        += character->speed * DeltaTime *
        Vector3(SmoothInput(GetAxisState(character->input, Input_Move_X)),
                SmoothInput(GetAxisState(character->input, Input_Move_Y)),
                0.0f);
}

Vector3 GetCharacterLocation(Character* character)
{
    return character->location;
}

void RenderCharacter(Character* character)
{
    glUseProgram(character_shader);
    glBindBuffer(GL_ARRAY_BUFFER, character_mesh);
    ApplyVertexDef(character_vert_def);

    Matrix4 local_to_world = Matrix4::translation(character->location);

    glUniformMatrix4fv(character_local_to_world_uniform, 1, false, (float*)&local_to_world);
    glUniform1i(character_sampler_uniform, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, character_texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    ClearVertexDef(character_vert_def);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void DestroyCharacter(Character* character)
{
    delete character;
}
