// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

attribute vec4 in_Position;
attribute vec4 in_Velocity;

varying vec4 out_Position;
varying vec4 out_Velocity;

void main(void)
{
  out_Position = in_Position;
  //out_Position.y -= 0.01;
  //if(out_Position.y < -1.0)
  //  out_Position.y += 2.0;
  out_Position += in_Velocity;
  out_Velocity = in_Velocity + vec4(0.00, -0.0001, 0.00, 0.00);
  gl_Position = out_Position;
}
