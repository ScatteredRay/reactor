// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

varying vec4 out_Position;

void main(void)
{
  out_Position = gl_Vertex;
  out_Position.y -= 0.01;
  if(out_Position.y < -1.0)
    out_Position.y += 2.0;
  gl_Position = out_Position;
}
