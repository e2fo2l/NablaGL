#include "Skybox.h"

#include <iostream>
#include <stdint.h>

#include <GL/glew.h>
#include <stb_image.h>
#include "utils/GLShader.h"

Skybox::Skybox() {
    VAO = 0;
    VBO = 0;
    EBO = 0;
    textureID = 0;
}

Skybox::~Skybox() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
    }
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
    }
    if (EBO != 0) {
        glDeleteBuffers(1, &EBO);
    }

    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
    }

    if (shader) {
        shader->Destroy();
        delete shader;
    }
}

void Skybox::Init(const std::array<std::string, 6> &textures) {
    /* Skybox cube */

    const float cubeVertices[] = {
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
    };

    const unsigned int cubeIndices[] = {
        2, 1, 0,
        0, 3, 2,
        4, 5, 6,
        6, 7, 4,
        7, 3, 0,
        0, 4, 7,
        6, 5, 1,
        1, 2, 6,
        6, 2, 3,
        3, 7, 6,
        0, 1, 5,
        5, 4, 0
    };

    /* Load in the shader */
    shader = new GLShader();
    shader->LoadVertexShader("assets/Shaders/skybox.vs");
    shader->LoadFragmentShader("assets/Shaders/skybox.fs");
    shader->Create();

    /* Load in the cube vertices */
    glBindVertexArray(0);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    auto program = shader->GetProgram();

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    int loc_position = glGetAttribLocation(program, "a_position");
    glEnableVertexAttribArray(loc_position);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* Load cubemap */
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    for (int i = 0; i < 6; i++) {
        int w, h, c;
        
        uint8_t *data = stbi_load(textures[i].c_str(), &w, &h, &c, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, /* sequence: +x,-x,+y,-y,+z,-z */
                         0, GL_SRGB8_ALPHA8,
                         w, h,
                         0, GL_RGBA, GL_UNSIGNED_BYTE,
                         data);
            stbi_image_free(data);
        } else {
            std::cout << "Failed to load " << textures[i] << " for the skybox\n";
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Skybox::Draw() {
    if(VAO == 0)
        return;
    
    /* Enable shader */
    glUseProgram(shader->GetProgram());

    /* Push vertices */
    glDepthFunc(GL_LEQUAL); // https://learnopengl.com/Advanced-OpenGL/Cubemaps
    /* Note: the above optimisation is not really useful here. Since there are transparent objects
       in the scene, the skybox has to be drawn first anyway. This could be useful if opaque objects
       were sorted and drawn first, but that isn't the case here. */

    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    GLint textureLocation = glGetUniformLocation(shader->GetProgram(), "u_sampler");
    glUniform1i(textureLocation, 0);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glDepthFunc(GL_LESS);
}

unsigned int Skybox::GetTID() {
    return this->textureID;
}

//くコ:彡