// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

varying vec4 out_Position;

void main(void)
{
  out_Position = gl_Vertex;
  gl_Position = out_Position;
}
