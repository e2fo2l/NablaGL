#pragma once

#include <string>
#include <vector>
#include <map>

#include "Mesh.h"
#include "utils/Vertex.h"

class GLShader;
class MeshOBJ : public Mesh {
    private:
        typedef struct {
            unsigned int elementStart = 0;
            unsigned int elementSize = 0;
            unsigned int materialID;
        } DrawShape;

        typedef struct {
            std::string diffuse;
            vec3 ambient;
            vec3 specular;
            float shininess;
        } Material;

        std::vector<Vertex> vertices;
        std::vector<Material> materials;
        std::map<std::string, unsigned int> textureMap;
        std::vector<DrawShape> drawShapes;

    public:
        MeshOBJ();
        virtual ~MeshOBJ();
        virtual bool LoadFile(const std::string& filename);
        virtual void Draw(GLShader* shader);
};

//くコ:彡