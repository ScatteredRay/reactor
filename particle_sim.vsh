// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

void main(void)
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}