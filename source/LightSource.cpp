#include "LightSource.h"

#include <iostream>
#include <GL/glew.h>
#include <stb_image.h>
#include "utils/Matrix.h"
#include "utils/GLShader.h"

LightSource::LightSource(const vec3& position,
                         const vec3& color,
                         float ambientIntensity,
                         float specularIntensity) {
        
    this->position = position;
    this->color = color;
    this->ambientIntensity = ambientIntensity;
    this->specularIntensity = specularIntensity;

    VAO = 0;
    VBO = 0;
    tID = 0;
}

void LightSource::Init() {
    float indicatorVertices[] = {
        /* Position*/  /* texcoords */
        -0.05f, -0.05f,    0.0f, 0.0f,
         0.05f, -0.05f,    1.0f, 0.0f,
        -0.05f,  0.05f,    0.0f, 1.0f,
         0.05f, -0.05f,    1.0f, 0.0f,
         0.05f,  0.05f,    1.0f, 1.0f,
        -0.05f,  0.05f,    0.0f, 1.0f
    };

    shader = new GLShader();
    shader->LoadVertexShader("assets/Shaders/billboard.vs");
    shader->LoadFragmentShader("assets/Shaders/billboard.fs");
    shader->Create();

    glBindVertexArray(0);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(indicatorVertices), indicatorVertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    /* Position */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);

    /* Texture coordinates */
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    int w, h, nc;
    stbi_set_flip_vertically_on_load(true);
    uint8_t *data = stbi_load("assets/Textures/light.png", &w, &h, &nc, 0);
    if (data) {
        glGenTextures(1, &tID);
        glBindTexture(GL_TEXTURE_2D, tID);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     nc == 3 ? GL_SRGB8 : GL_SRGB8_ALPHA8,
                     w, h, 0,
                     nc == 3 ? GL_RGB : GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
    }
    stbi_set_flip_vertically_on_load(false);
}

LightSource::~LightSource() {
    shader->Destroy();
    delete shader;

    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);
    if (tID != 0)
        glDeleteTextures(1, &tID);
}
    
vec3& LightSource::GetPosition() { return this->position; }
vec3& LightSource::GetColor() { return this->color; }
float& LightSource::GetAmbient() { return this->ambientIntensity; }
float& LightSource::GetSpecular() { return this->specularIntensity; }

LightSource::LightData LightSource::GetLightData() {
    LightData ld;
    ld.position = this->position;
    ld.ambientIntensity = this->ambientIntensity;
    ld.color = this->color;
    ld.specularIntensity = this->specularIntensity;
    return ld;
}

void LightSource::DrawIndicator(Camera &camera) {
    if(VAO == 0)
        return;

    auto program = shader->GetProgram();
    glUseProgram(program);
    glBindVertexArray(VAO);

    GLint textureLoc = glGetUniformLocation(program, "u_sampler");
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(textureLoc, 0);

    GLint positionLoc = glGetUniformLocation(program, "u_position");
    GLint cameraUpLoc = glGetUniformLocation(program, "u_cameraUp");
    GLint cameraRightLoc = glGetUniformLocation(program, "u_cameraRight");
    vec3 u = camera.GetUp();
    vec3 r = camera.GetRight();
    glUniform3f(positionLoc, position.x, position.y, position.z);
    glUniform3f(cameraUpLoc, u.x, u.y, u.z);
    glUniform3f(cameraRightLoc, r.x, r.y, r.z);

    glBindTexture(GL_TEXTURE_2D, tID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

//くコ:彡