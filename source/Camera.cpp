#include "Camera.h"

#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

float rad(float deg) { return deg * (float)M_PI / 180.0f; }

Camera::Camera() {}

Camera::Camera(const vec3 &startPos, const vec2 &startRot, float fov,
               float ratio, float near, float far, float sensivity,
               float speed) {

    position = startPos;
    rotation = startRot;
    this->fov = fov;
    this->ratio = ratio;
    this->near = near;
    this->far = far;
    this->sensivity = sensivity;
    this->speed = speed;

    upAxis = {0.0f, 1.0f, 0.0f};
}

void Camera::Translate(const vec3 &t) { this->position = this->position + t; }
void Camera::Rotate(const vec2 &r) {
    // Should implement operator overloading for vec2, but whatever
    this->rotation.x += r.x;
    this->rotation.y += r.y;
}

void Camera::UpdateTarget() {
    // Cap Pitch (to avoid camera flipping)
    if (rotation.y > 89.999f)
        rotation.y = 89.999f;
    if (rotation.y < -89.999f)
        rotation.y = -89.999f;

    rotation.x = fmod(rotation.x, 360.0f);

    vec3 dir;
    dir.x = cos(rad(rotation.x)) * cos(rad(rotation.y));
    dir.y = sin(rad(rotation.y));
    dir.z = sin(rad(rotation.x)) * cos(rad(rotation.y));
    target = normalise(dir);
}

void Camera::Update(float deltaT, bool up, bool down, bool left, bool right,
                    bool shift, const vec2 &mouseDelta) {

    // Rotation
    vec2 rotationDelta{0.0f, 0.0f};
    rotationDelta.x = mouseDelta.x * sensivity;
    rotationDelta.y = -mouseDelta.y * sensivity;
    Rotate(rotationDelta);
    UpdateTarget();

    // Translation
    vec3 movementDelta{0.0f, 0.0f, 0.0f};
    float movementSpeed = this->speed;

    if (shift)
        movementSpeed *= 2.5f;
    if (up)
        movementDelta = movementDelta + target;
    if (down)
        movementDelta = movementDelta - target;
    if (left)
        movementDelta = movementDelta - normalise(cross(target, upAxis));
    if (right)
        movementDelta = movementDelta + normalise(cross(target, upAxis));

    Translate(movementSpeed * deltaT * normalise(movementDelta));
}

mat4 Camera::ViewMatrix() {
    UpdateTarget();
    mat4 ret = mat4::lookAt(position, position + target, upAxis);
    return ret;
}

mat4 Camera::ProjectionMatrix() {
    return mat4::perspective(fov, ratio, near, far);
}
struct Camera::CameraMatrices Camera::GetCameraMatrices() {
    struct CameraMatrices ret;

    mat4 view = ViewMatrix();
    mat4 proj = ProjectionMatrix();
    for(int i = 0; i < 16; i++) {
        ret.view[i] = view.getData()[i];
        ret.projection[i] = proj.getData()[i];
    }

    return ret;
}

// Getters / Setters for ImGui
vec3 &Camera::GetPosition() { return this->position; }
vec2 &Camera::GetRotation() { return this->rotation; }
vec3 &Camera::GetUp() { return this->upAxis; }

const vec3 Camera::GetRight() {
    UpdateTarget();
    return normalise(cross(upAxis, target));
}


float &Camera::GetSensivity() { return this->sensivity; }
float &Camera::GetSpeed() { return this->speed; }

float &Camera::GetFOV() { return this->fov; }
float &Camera::GetRatio() { return this->ratio; }
float &Camera::GetNear() { return this->near; }
float &Camera::GetFar() { return this->far; }

//くコ:彡