// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include "editor_meshes.h"
#include "simple_mesh.h"
#include "simple_vectors.h"

#include <gl.h>
#include <assert.h>

struct Editor_Mesh
{
    GLuint gl_mesh;
    GLenum gl_primitive_type;
    GLsizei gl_index_count;
};

VertexDef editor_vertex_def;

struct editor_vert
{
    vector3 location;
};

VertexDef editor_vert_def()
{
    editor_vert* proxy = 0;
    VertexDef VD = CreateVertexDef(sizeof(editor_vert), 1);
    int i = 0;
    AddVertexAttribute(VD, i++, VERTEX_POSITION_ATTR, (size_t)&proxy->location, 3, GL_FLOAT);

    return VD;
}

void InitEditor()
{
    editor_vertex_def = editor_vert_def();
}

void CleanupEditor()
{
    DestroyVertexDef(editor_vertex_def);
}

Editor_Mesh* CreateGridMesh(unsigned int num_grid_lines, float grid_spacing)
{
    unsigned int num_verts = num_grid_lines * num_grid_lines * 2;
    editor_vert* verts = new editor_vert[num_verts];

    float extent = (num_grid_lines - 1) * grid_spacing / 2.0f;
    int i = 0;

    for(unsigned int y = 0; y < num_grid_lines; y++)
    {
        verts[i].location.x = -extent;
        verts[i].location.y = -extent + grid_spacing * y;
        verts[i].location.z = 0.0f;
        i++;

        verts[i].location.x = extent;
        verts[i].location.y = -extent + grid_spacing * y;
        verts[i].location.z = 0.0f;
        i++;
    }

    for(unsigned int x = 0; x < num_grid_lines; x++)
    {
        verts[i].location.x = -extent + grid_spacing * x;
        verts[i].location.y = -extent;
        verts[i].location.z = 0.0f;
        i++;

        verts[i].location.x = -extent + grid_spacing * x;
        verts[i].location.y = extent;
        verts[i].location.z = 0.0f;
        i++;
    }

    Editor_Mesh* mesh = new Editor_Mesh();
    mesh->gl_mesh = CreateMesh(num_verts, sizeof(editor_vert), verts);
    mesh->gl_index_count = num_verts;
    mesh->gl_primitive_type = GL_LINES;

    return mesh;
}

void DestroyMesh(Editor_Mesh* mesh)
{
    DestroyMesh(mesh->gl_mesh);
    delete mesh;
}

void DrawEditorMesh(Editor_Mesh* mesh)
{
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl_mesh);
    ApplyVertexDef(editor_vertex_def);

    glDrawArrays(mesh->gl_primitive_type, 0, mesh->gl_index_count);
}
