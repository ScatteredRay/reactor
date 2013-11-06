// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

attribute vec4 in_Position;
attribute vec4 in_Velocity;

varying vec4 out_Position;
varying vec4 out_Velocity;

#define PI 3.1415926535897932384626433832795

void main(void)
{
  out_Position = in_Position;
  //out_Position.y -= 0.01;
  //if(out_Position.y < -1.0)
  //  out_Position.y += 2.0;
  out_Position += in_Velocity;

  float a = PI/20.0;

  mat2 rotMat = mat2(cos(a), sin(a),
                     -sin(a), cos(a));

  out_Velocity = in_Velocity;


  out_Velocity.xy = rotMat * out_Velocity.xy;
  out_Velocity +=  vec4(0.00, -0.001, 0.00, 0.00);
  gl_Position = out_Position;
}
