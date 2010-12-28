#ifndef _CAMERA_H_
#define _CAMERA_H_

struct Camera;

#include "vectormath.h"

void SetCameraProjection(Camera* camera, float screen_width, float screen_height);

Camera* InitCamera(int screen_width, int screen_height);
void DestroyCamera(Camera* camera);
void UpdateCamera(Camera* camera, Vector3* tracking_points, unsigned int num_tracking_points);
Matrix4 CameraGetWorldToProjection(Camera* camera);

#endif //_CAMERA_H_
