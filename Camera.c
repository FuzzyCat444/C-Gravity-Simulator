#include "Camera.h"

#include <math.h>

void Camera_create(Camera* camera) {
    camera->pos = (Vec3) { 0.0, 10.0, 0.0 };
    camera->rot = (Vec3) { 0.0, 0.0, 0.0 };
    camera->focusedBodyId = -1;
}

void Camera_accelForward(Camera* camera, double amount) {
    camera->vel = vsum(camera->vel, vscl(Camera_forwardVector(camera), amount));
}

void Camera_accelRight(Camera* camera, double amount) {
    camera->vel = vsum(camera->vel, vscl(Camera_rightVector(camera), amount));
}

void Camera_accelUp(Camera* camera, double amount) {
    camera->vel = vsum(camera->vel, vscl(Camera_upVector(camera), amount));
}

void Camera_move(Camera* camera, double dt) {
    camera->pos = vsum(camera->pos, vscl(camera->vel, dt));
}

Vec3 Camera_forwardVector(Camera* camera) {
    double yaw = camera->rot.y;
    double cosYaw = cos(yaw);
    double sinYaw = sin(yaw);
    Vec3 forward = vrotzx((Vec3) { 0.0, 0.0, -1.0 }, cosYaw, sinYaw);
    return forward;
}

Vec3 Camera_rightVector(Camera* camera) {
    double yaw = camera->rot.y;
    double cosYaw = cos(yaw);
    double sinYaw = sin(yaw);
    Vec3 right = vrotzx((Vec3) { 1.0, 0.0, 0.0 }, cosYaw, sinYaw);
    return right;
}

Vec3 Camera_upVector(Camera* camera) {
    return (Vec3) { 0.0, 1.0, 0.0 };
}

Vec3 Camera_pixelToRay(Camera* camera, double x, double y, double width, double height, double fov) {
    double sizeY = 2.0 * tan(fov / 2.0);
    double sizeX = sizeY * width / height;
    
    double nearX = (x / width - 0.5) * sizeX;
    double nearY = -(y / height - 0.5) * sizeY;
    
    double yaw = camera->rot.y;
    double pitch = camera->rot.x;
    double cosYaw = cos(yaw);
    double sinYaw = sin(yaw);
    double cosPitch = cos(pitch);
    double sinPitch = sin(pitch);
    
    Vec3 ray = vnorm((Vec3) { nearX, nearY, -1.0 });
    ray = vrotyz(ray, cosPitch, sinPitch);
    ray = vrotzx(ray, cosYaw, sinYaw);
    
    return ray;
}