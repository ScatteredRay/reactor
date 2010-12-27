#ifndef _OBJ_LOAD_H_
#define _OBJ_LOAD_H_

struct obj_vert
{
    vector3 location;
};

VertexDef obj_vert_def();

struct obj_face
{
    unsigned short a, b, c;
};

struct obj_mesh
{
    unsigned int vertex_count;
    obj_vert* verticies;
    unsigned int face_count;
    obj_face* faces;
    ~obj_mesh()
    {
        delete [] verticies;
        delete [] faces;
    }
};

obj_mesh* obj_load_mesh(const char* filename);

#endif //_OBJ_LOAD_H_
