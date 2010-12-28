// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

uniform mat4 local_to_world;

void main(void)
{
    gl_Position = gl_ModelViewProjectionMatrix * local_to_world * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
