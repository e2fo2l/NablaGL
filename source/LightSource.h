#pragma once

#include "Camera.h"
#include "utils/Vertex.h"


class GLShader;

/* Omnidirectional light */
class LightSource {
private:
    vec3 position;
    vec3 color;
    float ambientIntensity;
    float specularIntensity;

    unsigned int VAO, VBO;
    unsigned int tID;
    GLShader* shader;

public:
    typedef struct {
        /* Data formatted to respect std140 */
        vec3 position;
        float ambientIntensity; /* not used */
        vec3 color;
        float specularIntensity;
    } LightData;

    LightSource(const vec3& position = {0.0f, 0.0f, 0.0f},
                const vec3& color    = {1.0f, 1.0f, 1.0f},
                float ambientIntensity = 1.0f,
                float specularIntensity = 1.0f);
    ~LightSource();

    void Init();
    
    vec3& GetPosition();
    vec3& GetColor();
    float& GetAmbient();
    float& GetSpecular();
    LightData GetLightData();

    void DrawIndicator(Camera& camera);
};

//くコ:彡