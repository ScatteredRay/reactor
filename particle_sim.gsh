// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#version 420

layout(points) in;
layout(points, max_vertices=1) out;

layout (binding = 0) uniform atomic_uint particle_count;

in vec4 vec_Position[];
in vec4 vec_Velocity[];

out vec4 out_Position;
out vec4 out_Velocity;

void main(void)
{
  out_Position = vec_Position[0];
  out_Velocity = vec_Velocity[0];
  gl_Position = gl_in[0].gl_Position;
  gl_PointSize = 1.0;
  if(gl_Position.y > -1.0 && gl_Position.y < 1.0 &&
     gl_Position.x > -1.0 && gl_Position.x < 1.0)
    EmitVertex();
  else
    atomicCounterDecrement(particle_count);
}
