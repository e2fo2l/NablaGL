#include "Mesh.h"

#include <GL/glew.h>
#include <stb_image.h>
#include "utils/GLShader.h"

Mesh::Mesh() {
    VAO = 0;
    VBO = 0;
    EBO = 0;
    tID = 0;
    elementCount = 0;
}

Mesh::~Mesh() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
    }
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
    }
    if (EBO != 0) {
        glDeleteBuffers(1, &EBO);
    }

    if (tID != 0) {
        glDeleteTextures(1, &tID);
    }
}

void Mesh::LoadMesh(const std::vector<float>& vertices,
                    const std::vector<unsigned int>& indices,
                    const std::string &texture,
                    unsigned int skyboxID) {

    
    elementCount = indices.size(); // 3 floats for vertex, 3 floats for normal, 2 floats for texcoord
    
    glBindVertexArray(0);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    /* Vertices */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);

    /* Normals */
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));

    /* Texture coords */
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if (!texture.empty()) {
        int w, h, nc;
        stbi_set_flip_vertically_on_load(true);
        uint8_t* data = stbi_load(texture.c_str(), &w, &h, &nc, 0);
        if (data) {
            glGenTextures(1, &tID);
            glBindTexture(GL_TEXTURE_2D, tID);
            glTexImage2D(GL_TEXTURE_2D, 0,
                         nc == 3 ? GL_SRGB8 : GL_SRGB8_ALPHA8,
                         w, h, 0,
                         nc == 3 ? GL_RGB : GL_RGBA,
                         GL_UNSIGNED_BYTE, data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            stbi_image_free(data);
        }
        stbi_set_flip_vertically_on_load(false);
    }

    this->skyboxID = skyboxID;
}

void Mesh::Draw(GLShader *shader) {
    if(VAO == 0 || elementCount == 0)
        return;

    auto program = shader->GetProgram();
    glUseProgram(program);
    glBindVertexArray(VAO);

    GLint Ka = glGetUniformLocation(program, "u_Ka");
    GLint Kd = glGetUniformLocation(program, "u_Kd");
    GLint Ks = glGetUniformLocation(program, "u_Ks");
    GLint Ns = glGetUniformLocation(program, "u_Ns");
    GLint skyboxLoc = glGetUniformLocation(program, "u_skybox");
    glUniform1i(Kd, 0);
    glUniform1i(skyboxLoc, 1);

    // Default material
    glUniform3f(Ka, 0.1f, 0.1f, 0.1f);
    glUniform3f(Ks, 1.0f, 1.0f, 1.0f);
    glUniform1f(Ns, 32.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tID);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);

    glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, 0);
}

//くコ:彡