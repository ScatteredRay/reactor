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

void parse_face_index(char** c, unsigned short* v, unsigned short* n, unsigned short* t)
{
    char* curr = *c;

    while(is_whitespace(*curr))
        curr++;

    int i;
    i = parse_int(&curr) - 1; // location index
    if(v) *v = i;
    assert(*curr == '/');
    curr++;
    i = parse_int(&curr) - 1; // normal index
    if(n) *n = i;
    assert(*curr == '/');
    curr++;
    i = parse_int(&curr) - 1; // tangent index
    if(t) *t = i;

    *c = curr;
}

obj_mesh* obj_load_mesh(const char* filename)
{
    char* buffer = load_entire_file(filename, "r");

    if(!buffer)
        return NULL;

    // Build size counts
    char* curr = buffer;

    unsigned int vertex_count = 0;
    unsigned int face_count = 0;

    while(*curr != '\0')
    {
        if(is_whitespace(*curr))
        {
            curr++;
            continue;
        }

        if(check_match(curr, "v "))
            vertex_count++;
        else if(check_match(curr, "f "))
            face_count++;

        skip_line(&curr);

    }


    printf("Vertices: %d, Faces: %d.\n", vertex_count, face_count);

    obj_vert* verticies = new obj_vert[vertex_count];

    obj_face* faces = new obj_face[face_count];

    unsigned int vidx = 0;
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
            assert(vidx < vertex_count);
            verticies[vidx].location.x = parse_float(&curr);
            verticies[vidx].location.y = parse_float(&curr);
            verticies[vidx].location.z = parse_float(&curr);
            vidx++;
            skip_line(&curr);
        }
        else if(check_match(curr, "f "))
        {
            curr++;
            assert(fidx < face_count);
            parse_face_index(&curr, &faces[fidx].a, NULL, NULL);
            parse_face_index(&curr, &faces[fidx].b, NULL, NULL);
            parse_face_index(&curr, &faces[fidx].c, NULL, NULL);
            fidx++;
            skip_line(&curr);
        }
        else
            skip_line(&curr);

    }

    obj_mesh* mesh = new obj_mesh();

    mesh->vertex_count = vertex_count;
    mesh->verticies = verticies;
    mesh->face_count = face_count;
    mesh->faces = faces;

    delete [] buffer;

    return mesh;
}

#include <gl.h>

VertexDef obj_vert_def()
{
    obj_vert* proxy = 0;
    VertexDef VD = CreateVertexDef(sizeof(obj_vert), 1);
    int i = 0;
    AddVertexAttribute(VD, i++, VERTEX_POSITION_ATTR, (size_t)&proxy->location, 3, GL_FLOAT);

    return VD;
}
