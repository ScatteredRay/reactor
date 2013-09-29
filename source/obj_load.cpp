#include <stdio.h>
#ifdef _WIN32
#include <malloc.h>
//#define alloca _alloca
#else
#include <alloca.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "simple_mesh.h"
#include "simple_vectors.h"
#include "obj_load.h"
#include "file.h"

bool is_newline(char c)
{
    return (c == '\r') || ( c == '\n');
}

bool is_whitespace(char c)
{
    return (c == ' ') || (c == '\t') || is_newline(c);
}

bool is_delimiter(char c)
{
    return is_whitespace(c) || (c == '(') || (c == ')') || (c == '\0');
}

bool is_number_char(char c)
{
    return (c >= '0') && (c <= '9');
}

bool is_alpha_char(char c)
{
    return
        ((c >= 'a') && (c <= 'z')) ||
        ((c >= 'A') && (c <= 'Z'));
}

bool is_symbol_char(char c)
{
    return is_alpha_char(c);
}

bool is_extended_hex_char(char c)
{
    return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool check_match(char* curr, const char* cmp)
{
    while(*cmp != '\0')
    {
        if(*curr == '\0')
            return false;

        if(*cmp != *curr)
            return false;

        curr++;
        cmp++;
    }

    if(is_whitespace(*curr))
        return true;
    else
        return false;
}

void skip_line(char** curr)
{
    while(**curr != '\0')
    {
        if(**curr == '\n')
        {
            (*curr)++;
            return;
        }
        (*curr)++;
    }
}

float parse_float(char** c)
{
    char* curr = *c;

    while(is_whitespace(*curr))
        curr++;
    
    if(!is_number_char(*curr) && *curr != '-' && *curr != '.')
        return 0.0f;

    char* start = curr;
    
    while(is_number_char(*curr) || *curr == '.' || *curr == '-')
    {
        curr++;
    }

    char* buffer = (char*)alloca((curr + 1 - start) * sizeof(char));
    strncpy(buffer, start, (curr - start));
    buffer[curr-start] = '\0';

    *c = curr;

    return atof(buffer);
}

int parse_int(char** c)
{
    char* curr = *c;

    while(is_whitespace(*curr))
        curr++;
    
    if(!is_number_char(*curr))
        return 0;

    char* start = curr;
    
    while(is_number_char(*curr))
    {
        curr++;
    }

    char* buffer = (char*)alloca((curr + 1 - start) * sizeof(char));
    strncpy(buffer, start, (curr - start));
    buffer[curr-start] = '\0';

    *c = curr;

    return strtol(buffer, NULL, 10);
}

char* parse_symbol(char** c)
{
    char* curr = *c;

    while(is_whitespace(*curr))
        curr++;

    if(!is_symbol_char(*curr))
        return NULL;
    
    char* start = curr;

    while(is_symbol_char(*curr))
    {
        curr++;
    }

    char* buffer = new char[(curr + 1 - start) * sizeof(char)];
    strncpy(buffer, start, (curr - start));
    buffer[curr-start] = '\0';

    *c = curr;

    return buffer;
}

// Very minimal obj parser, isn't very robust and loads the entire thing file at a time,
// should do for our job though.

void parse_face_index(char** c, unsigned short* v, unsigned short* t, unsigned short* n)
{
    char* curr = *c;

    while(is_whitespace(*curr))
        curr++;

    int i;
    i = parse_int(&curr) - 1; // location index
    if(v) *v = i;
    assert(*curr == '/');
    curr++;
    i = parse_int(&curr) - 1; // texture index
    if(t) *t = i;
    assert(*curr == '/');
    curr++;
    i = parse_int(&curr) - 1; // normal index
    if(n) *n = i;

    *c = curr;
}

void populate_face(char** c, obj_vert* vert, vector3* positions, vector2* uvs, vector3* normals, material_params& params)
{
    unsigned short position, uv, normal;
    parse_face_index(c,
                     &position,
                     &uv,
                     &normal);

    vert->location = positions[position];
    vert->uv = uvs[uv];
    vert->normal = normals[normal];
    vert->diffuse = params.diffuse;
    vert->ambient = params.ambient;
    vert->specular.x = params.specular.x;
    vert->specular.y = params.specular.y;
    vert->specular.z = params.specular.z;
    vert->specular.w = params.specular_power;
    vert->emissive.x = params.emissive.x;
    vert->emissive.y = params.emissive.y;
    vert->emissive.z = params.emissive.z;
    vert->emissive.w = params.emissive_brightness;
}

bool load_ssh(const char* shname, material_params* params);

obj_mesh* obj_load_mesh(const char* filename)
{
    char* buffer = load_entire_file(filename, "r");

    if(!buffer)
        return NULL;

    // Build size counts
    char* curr = buffer;

    unsigned int position_count = 0;
    unsigned int uv_count = 0;
    unsigned int normal_count = 0;
    unsigned int face_count = 0;

    while(*curr != '\0')
    {
        if(is_whitespace(*curr))
        {
            curr++;
            continue;
        }

        if(check_match(curr, "v"))
            position_count++;
        else if(check_match(curr, "vt"))
            uv_count++;
        else if(check_match(curr, "vn"))
            normal_count++;
        else if(check_match(curr, "f"))
            face_count++;

        skip_line(&curr);

    }


    printf("Vertices: %d, Faces: %d.\n", position_count, face_count);

    vector3* positions = new vector3[position_count];
    vector2* uvs = new vector2[uv_count];
    vector3* normals = new vector3[normal_count];
    obj_vert* verticies = new obj_vert[face_count*3];

    unsigned int pidx = 0;
    unsigned int uvidx = 0;
    unsigned int nidx = 0;
    unsigned int fidx = 0;

    // Actually parse the obj file

    material_params current_params;
    load_ssh("Default", &current_params);

    curr = buffer;

    while(*curr != '\0')
    {
        if(is_whitespace(*curr))
        {
            curr++;
            continue;
        }

        if(check_match(curr, "v"))
        {
            curr++;
            assert(pidx < position_count);
            positions[pidx].x = parse_float(&curr);
            positions[pidx].y = parse_float(&curr);
            positions[pidx].z = parse_float(&curr);
            pidx++;
            skip_line(&curr);
        }
        else if(check_match(curr, "vt"))
        {
            curr += 2;
            assert(uvidx < uv_count);
            uvs[uvidx].x = parse_float(&curr);
            uvs[uvidx].y = parse_float(&curr);
            uvidx++;
            skip_line(&curr);
        }
        else if(check_match(curr, "vn"))
        {
            curr += 2;
            assert(nidx < normal_count);

            normals[nidx].x = parse_float(&curr);
            normals[nidx].y = parse_float(&curr);
            normals[nidx].z = parse_float(&curr);
            nidx++;
            skip_line(&curr);
        }
        else if(check_match(curr, "f"))
        {
            curr++;
            assert(fidx+2 < face_count*3);

            populate_face(&curr, &verticies[fidx++], positions, uvs, normals, current_params);
            populate_face(&curr, &verticies[fidx++], positions, uvs, normals, current_params);
            populate_face(&curr, &verticies[fidx++], positions, uvs, normals, current_params);

            skip_line(&curr);
        }
        else if(check_match(curr, "usemtl"))
        {
            curr += 6;
            char* material = parse_symbol(&curr);
            if(material)
            {
                if(!load_ssh(material, &current_params))
                        load_ssh("Default", &current_params);
                delete [] material;
            }
            skip_line(&curr);
        }
        else
            skip_line(&curr);

    }

    obj_mesh* mesh = new obj_mesh();

    mesh->vertex_count = face_count*3;
    mesh->verticies = verticies;

    delete [] positions;
    delete [] uvs;
    delete [] normals;

    delete [] buffer;

    return mesh;
}

// Parse shading files

struct param_parse_map
{
    const char* symbol;
    size_t offset;
    unsigned int count;
};

const param_parse_map material_parse_mapping[] =
{{"Diffuse", (size_t)&((material_params*)NULL)->diffuse, 3},
 {"Ambient", (size_t)&((material_params*)NULL)->ambient, 3},
 {"Specular", (size_t)&((material_params*)NULL)->specular, 3},
 {"SpecularPower", (size_t)&((material_params*)NULL)->specular_power, 1},
 {"Emissive", (size_t)&((material_params*)NULL)->emissive, 3},
 {"EmissiveBrightness", (size_t)&((material_params*)NULL)->emissive_brightness, 1},
 {NULL, 0, 0}};

bool load_ssh(const char* shname, material_params* params)
{
    const char* shpath = "data/world/materials/";
    const char* shext = ".ssh";
    char* filename = new char[strlen(shname) +
                              strlen(shpath) +
                              strlen(shext) + 1];

    strcpy(filename, shpath);
    strcpy(filename + strlen(filename), shname);
    strcpy(filename + strlen(filename), shext);

    char* buffer = load_entire_file(filename, "r");
    delete filename;

    if(!buffer)
        return false;

    char* curr = buffer;

    while(*curr != '\0')
    {
        if(is_whitespace(*curr))
        {
            curr++;
            continue;
        }
        
        if(*curr == '#')
        {
            skip_line(&curr);
            continue;
        }

        const param_parse_map* P = material_parse_mapping;
        while(P->symbol)
        {
            if(check_match(curr, P->symbol))
            {
                curr += strlen(P->symbol);
                for(unsigned int i=0; i<P->count; i++)
                {
                    ((float*)((char*)params+P->offset))[i] = parse_float(&curr);
                }
                skip_line(&curr);
                break;
            }
            P++;
        }

        if(!P->symbol)
        {
            printf("Unparsable line while parsing material: %s\n", shname);
            skip_line(&curr);
        }
            
    }

    //printf("Material Diffuse: %f %f %f\n", params->diffuse.x, params->diffuse.y, params->diffuse.z);

    delete [] buffer;

    return true;
}

#include "gl_all.h"

VertexDef obj_vert_def()
{
    obj_vert* proxy = 0;
    VertexDef VD = CreateVertexDef(sizeof(obj_vert), 7);
    int i = 0;
    AddVertexAttribute(VD, i++, VERTEX_OTHER_ATTR, (size_t)&proxy->location, 3, GL_FLOAT, "in_vertex");
    AddVertexAttribute(VD, i++, VERTEX_OTHER_ATTR, (size_t)&proxy->normal, 3, GL_FLOAT, "in_normal");
    AddVertexAttribute(VD, i++, VERTEX_OTHER_ATTR, (size_t)&proxy->uv, 2, GL_FLOAT, "in_uv");
    AddVertexAttribute(VD, i++, VERTEX_OTHER_ATTR, (size_t)&proxy->diffuse, 3, GL_FLOAT, "in_color");
    AddVertexAttribute(VD, i++, VERTEX_OTHER_ATTR, (size_t)&proxy->ambient, 3, GL_FLOAT, "in_ambient");
    AddVertexAttribute(VD, i++, VERTEX_OTHER_ATTR, (size_t)&proxy->specular, 4, GL_FLOAT, "in_specular");
    AddVertexAttribute(VD, i++, VERTEX_OTHER_ATTR, (size_t)&proxy->emissive, 4, GL_FLOAT, "in_emissive");

    return VD;
}

// Depricated.
VertexDef obj_vert_def_depr()
{
    obj_vert* proxy = 0;
    VertexDef VD = CreateVertexDef(sizeof(obj_vert), 7);
    int i = 0;
    AddVertexAttribute(VD, i++, VERTEX_POSITION_ATTR, (size_t)&proxy->location, 3, GL_FLOAT);
    AddVertexAttribute(VD, i++, VERTEX_NORMAL_ATTR, (size_t)&proxy->normal, 3, GL_FLOAT);
    AddVertexAttribute(VD, i++, VERTEX_UV_ATTR, (size_t)&proxy->uv, 2, GL_FLOAT);
    AddVertexAttribute(VD, i++, VERTEX_COLOR_ATTR, (size_t)&proxy->diffuse, 3, GL_FLOAT);
    AddVertexAttribute(VD, i++, VERTEX_OTHER_ATTR_0, (size_t)&proxy->ambient, 3, GL_FLOAT);
    AddVertexAttribute(VD, i++, VERTEX_OTHER_ATTR_1, (size_t)&proxy->specular, 4, GL_FLOAT);
    AddVertexAttribute(VD, i++, VERTEX_OTHER_ATTR_2, (size_t)&proxy->emissive, 4, GL_FLOAT);

    return VD;
}
