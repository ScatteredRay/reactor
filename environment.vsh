// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

varying vec4 position;
varying vec4 color;
varying vec4 normal;

varying vec3 vambient;
varying vec4 vspecular;
varying vec4 vemissive;

attribute vec3 ambient;
attribute vec4 specular;
attribute vec4 emissive;

void main(void)
{
    gl_Position = position = gl_ModelViewProjectionMatrix * gl_Vertex;
    color = gl_Color;
    normal = gl_ProjectionMatrix * vec4(gl_Normal, 1.0);

    vambient = ambient;
    vspecular = specular;
    vemissive = emissive;
}
