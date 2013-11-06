// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

varying vec2 texCoord;

void main(void)
{
    gl_Position = gl_Vertex;
    texCoord = gl_MultiTexCoord0.xy;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
