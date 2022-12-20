#pragma once

#include <array>
#include <string>
#include "utils/Matrix.h"

class GLShader;

class Skybox {
    private:
        unsigned int textureID;
        unsigned int VAO, VBO, EBO;
        GLShader *shader = nullptr;

    public:
        Skybox();
        ~Skybox();
        void Init(const std::array<std::string, 6>& textures);
        void Draw();

        unsigned int GetTID();
};

//くコ:彡