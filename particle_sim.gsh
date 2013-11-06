// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#version 420

layout(points) in;
layout(points, max_vertices=1) out;

in vec4 vec_Position[];
in vec4 vec_Velocity[];

out vec4 out_Position;
out vec4 out_Velocity;

void main(void)
{
  out_Position = vec_Position[0];
  out_Velocity = vec_Velocity[0];
  gl_Position = gl_in[0].gl_Position;
  EmitVertex();
}
