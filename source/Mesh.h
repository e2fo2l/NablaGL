#pragma once

#include <string>
#include <vector>

class GLShader;
class Mesh {
  protected:
    unsigned int VAO, VBO, EBO;
    unsigned int tID, skyboxID;
    unsigned int elementCount;

  public:
    Mesh();
    virtual ~Mesh();
    virtual void LoadMesh(const std::vector<float>& vertices,
                          const std::vector<unsigned int>& indices,
                          const std::string& texture,
                          unsigned int skyboxID);
    virtual void Draw(GLShader *shader);
};

//くコ:彡