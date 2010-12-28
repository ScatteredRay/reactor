#include "vectormath.h"
#include <stdio.h>

const Vector3 camera_start_location = Vector3(0.0, 0.0, 85.0);
const Vector3 camera_up_vector = Vector3(0.0, 1.0, 0.0);
const float fovx = (40.0f / 360.0f) * 2 * PI;
const float zNear = 10.0f;
const float zFar = 500.0f; //1000.0f to get background plane

struct Camera
{
    Vector3 location;
    Matrix4 projection;
};

void SetCameraProjection(Camera* camera, float screen_width, float screen_height)
{
    float aspect = screen_width/screen_height;
    camera->projection = Matrix4::perspective(fovx*screen_height/screen_width, aspect, zNear, zFar);
}

Camera* InitCamera(int screen_width, int screen_height)
{
    Camera* camera = new Camera();

    camera->location = camera_start_location;

    SetCameraProjection(camera, (float)screen_width, (float)screen_height);

    return camera;
}

void DestroyCamera(Camera* camera)
{
    delete camera;
}

void UpdateCamera(Camera* camera, Vector3* tracking_points, unsigned int num_tracking_points)
{
    Vector3 location = camera->location;
    for(unsigned int i=0; i<num_tracking_points; i++)
    {
        location += tracking_points[i];
    }

    if(num_tracking_points)
    {
        location /= num_tracking_points + 1;
        location.setZ(camera->location.getZ());
    }

    camera->location = location;
}

Matrix4 CameraGetWorldToProjection(Camera* camera)
{
    Vector3 look_at = camera->location;
    look_at.setZ(0.0f);
    Matrix4 world_to_view = Matrix4::lookAt(
        Point3(camera->location),
        Point3(look_at),
        camera_up_vector);

    return camera->projection * world_to_view;
}
