// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

attribute vec4 in_Position;
attribute vec4 in_Velocity;

varying vec4 vec_Position;
varying vec4 vec_Velocity;

uniform sampler2D scene_depth;

uniform mat4 view_projection;
uniform mat4 inv_view_projection;

#define PI 3.1415926535897932384626433832795

const mat4 screenToTex = mat4(
  0.5, 0.0, 0.0, 0.0,
  0.0, 0.5, 0.0, 0.0,
  0.0, 0.0, 0.5, 0.0,
  0.5, 0.5, 0.5, 1.0
);

void main(void)
{
  vec_Position = view_projection * in_Position;
  vec_Position /= vec_Position.w;
  //vec_Position.y -= 0.01;
  //if(vec_Position.y < -1.0)
  //  vec_Position.y += 2.0;
  vec_Position += in_Velocity;

  vec4 depth = texture2D(scene_depth, (screenToTex * vec_Position).xy);

  float a = PI/20.0;

  mat2 rotMat = mat2(cos(a), sin(a),
                     -sin(a), cos(a));

  vec_Velocity = in_Velocity * depth;


  vec_Velocity.xy = rotMat * vec_Velocity.xy;
  vec_Velocity +=  vec4(0.00, -0.001, 0.00, 0.00);
  gl_Position = vec_Position;
}
