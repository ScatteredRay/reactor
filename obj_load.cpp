#include <stdio.h>
#include <alloca.h>
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

    return true;
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

void populate_face(char** c, obj_vert* vert, vector3* positions, vector2* uvs, vector3* normals)
{
    unsigned short position, uv, normal;
    parse_face_index(c,
                     &position,
                     &uv,
                     &normal);

    vert->location = positions[position];
    vert->uv = uvs[uv];
    vert->normal = normals[normal];
    vert->diffuse.x = 0.0f;
    vert->diffuse.y = 1.0f;
    vert->diffuse.z = 0.0f;
}

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

        if(check_match(curr, "v "))
            position_count++;
        else if(check_match(curr, "vt "))
            uv_count++;
        else if(check_match(curr, "vn "))
            normal_count++;
        else if(check_match(curr, "f "))
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

    curr = buffer;

    while(*curr != '\0')
    {
        if(is_whitespace(*curr))
        {
            curr++;
            continue;
        }

        if(check_match(curr, "v "))
        {
            curr++;
            assert(pidx < position_count);
            positions[pidx].x = parse_float(&curr);
            positions[pidx].y = parse_float(&curr);
            positions[pidx].z = parse_float(&curr);
            pidx++;
            skip_line(&curr);
        }
        else if(check_match(curr, "vt "))
        {
            curr += 2;
            assert(uvidx < uv_count);
            uvs[uvidx].x = parse_float(&curr);
            uvs[uvidx].y = parse_float(&curr);
            uvidx++;
            skip_line(&curr);
        }
        else if(check_match(curr, "vn "))
        {
            curr += 2;
            assert(nidx < normal_count);

            normals[nidx].x = parse_float(&curr);
            normals[nidx].y = parse_float(&curr);
            normals[nidx].z = parse_float(&curr);
            nidx++;
            skip_line(&curr);
        }
        else if(check_match(curr, "f "))
        {
            curr++;
            assert(fidx+2 < face_count*3);

            populate_face(&curr, &verticies[fidx++], positions, uvs, normals);
            populate_face(&curr, &verticies[fidx++], positions, uvs, normals);
            populate_face(&curr, &verticies[fidx++], positions, uvs, normals);

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

#include <gl.h>

VertexDef obj_vert_def()
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
