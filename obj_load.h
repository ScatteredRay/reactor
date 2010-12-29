#ifndef _OBJ_LOAD_H_
#define _OBJ_LOAD_H_

struct obj_vert
{
    vector3 location;
    vector2 uv;
    vector3 normal;

    vector3 diffuse;
    vector3 ambient;
    vector4 specular;
    vector4 emissive;
};

VertexDef obj_vert_def();

struct obj_mesh
{
    unsigned int vertex_count;
    obj_vert* verticies;
    ~obj_mesh()
    {
        delete [] verticies;
    }
};

obj_mesh* obj_load_mesh(const char* filename);

#endif //_OBJ_LOAD_H_
