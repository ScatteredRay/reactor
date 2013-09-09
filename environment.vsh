// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

varying vec4 position;
varying vec4 color;
varying vec4 normal;

varying vec3 vambient;
varying vec4 vspecular;
varying vec4 vemissive;

attribute vec4 in_vertex;
attribute vec4 in_color;
attribute vec3 in_normal;
attribute vec3 in_ambient;
attribute vec4 in_specular;
attribute vec4 in_emissive;

void main(void)
{
    gl_Position = position = gl_ModelViewProjectionMatrix * in_vertex;
    color = in_color;
    // This should be in view space, since we don't do any rotations,
    // it is in such a space already, if we need to start rotating things
    // we will have to deal with this.
    normal = vec4(in_normal, 1.0);

    vambient = in_ambient;
    vspecular = in_specular;
    vemissive = in_emissive;
}
