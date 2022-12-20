#pragma once

#include "utils/Matrix.h"
#include "utils/Vertex.h"

class Camera {
    private:
        // View matrix
        vec3 position;
        vec3 target;
        vec3 upAxis;

        vec2 rotation; // x: yaw, y: pitch

        float sensivity;
        float speed;

        // Projection matrix
        float fov;
        float ratio;
        float near;
        float far;

        // TODO: cache the matrix, do that it's not computed every frame
        void UpdateTarget();

    public:
        struct CameraMatrices {
            float view[16];
            float projection[16];
        };

        Camera();
        Camera(const vec3& startPos, const vec2& startRot,
               float fov, float ratio, float near, float far,
               float sensivity = 0.075f, float speed = 0.75f);

        void Translate(const vec3& t);
        void Rotate(const vec2& r);
        
        void Update(float deltaT,
                    bool up, bool down, bool left, bool right, bool shift,
                    const vec2& deltaMouse);
        
        mat4 ViewMatrix();
        mat4 ProjectionMatrix();
        struct CameraMatrices GetCameraMatrices();


        // Getters / Setters for ImGui
        vec3& GetPosition();        
        vec2& GetRotation();
        vec3& GetUp();
        const vec3 GetRight();
        
        float& GetSensivity();
        float& GetSpeed();
        
        float& GetFOV();
        float& GetRatio();
        float& GetNear();
        float& GetFar();
};

//くコ:彡