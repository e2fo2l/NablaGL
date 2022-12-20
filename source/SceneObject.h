#pragma once

#include "utils/Vertex.h"

class Mesh;
class GLShader;

class SceneObject {
    private:
        vec3 position;
        vec3 rotation;
        vec3 scale;

        Mesh *mesh = NULL;
        GLShader *shader = NULL;
    public:
        SceneObject(Mesh *mesh, GLShader *shader,
                    const vec3& position = {0.0f, 0.0f, 0.0f},
                    const vec3& rotation = {0.0f, 0.0f, 0.0f},
                    const vec3& scale =    {1.0f, 1.0f, 1.0f});
        Mesh*& GetMesh();
        GLShader*& GetShader();

        vec3& GetPosition();
        vec3& GetRotation();
        vec3& GetScale();

        void Translate(const vec3& t);
        void Rotate(const vec3& r);
        void Scale(const vec3& s);

        void Draw();
};

//くコ:彡