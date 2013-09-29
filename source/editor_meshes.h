// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _EDITOR_MESHES_
#define _EDITOR_MESHES_

struct Editor_Mesh;

void InitEditor();
void CleanupEditor();

// Grid spans as a square in the XY axis, centered on 0,0,0.
Editor_Mesh* CreateGridMesh(unsigned int num_grid_lines, float grid_spacing);

void DestroyMesh(Editor_Mesh* mesh);
void DrawEditorMesh(Editor_Mesh* mesh);

#endif //_EDITOR_MESHES_

