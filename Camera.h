#ifndef CAMERA_H
#define CAMERA_H

#include "Vec.h"

typedef struct Camera {
    Vec3 pos;
    Vec3 vel;
    Vec3 rot;
    int focusedBodyId;
} Camera;

void Camera_create(Camera* camera);

void Camera_accelForward(Camera* camera, double amount);

void Camera_accelRight(Camera* camera, double amount);

void Camera_accelUp(Camera* camera, double amount);

void Camera_move(Camera* camera, double dt);

Vec3 Camera_forwardVector(Camera* camera);

Vec3 Camera_rightVector(Camera* camera);

Vec3 Camera_upVector(Camera* camera);

Vec3 Camera_pixelToRay(Camera* camera, double x, double y, double width, double height, double fov);

#endif