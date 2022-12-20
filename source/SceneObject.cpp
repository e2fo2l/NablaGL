#include "SceneObject.h"

#include <math.h>
#include <GL/glew.h>
#include "utils/Matrix.h"
#include "utils/GLShader.h"
#include "Mesh.h"

SceneObject::SceneObject(Mesh *mesh, GLShader *shader,
            const vec3 &position,
            const vec3 &rotation,
            const vec3 &scale) {

    this->mesh = mesh;
    this->shader = shader;
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
}

Mesh *&SceneObject::GetMesh() { return this->mesh; }
GLShader *&SceneObject::GetShader() { return this->shader; }

vec3 &SceneObject::GetPosition() { return this->position; }
vec3 &SceneObject::GetRotation() { return this->rotation; }
vec3 &SceneObject::GetScale() { return this->scale; }

void SceneObject::Translate(const vec3 &t) { position = position + t; }
void SceneObject::Rotate(const vec3 &r) { rotation = rotation + r; }
void SceneObject::Scale(const vec3 &s) { scale = scale + s; }

void SceneObject::Draw() {
    if(!shader || !mesh) /* Invalid shader/model */
        return;

    rotation.x = fmod(rotation.x, 360.0f);
    rotation.y = fmod(rotation.y, 360.0f);
    rotation.z = fmod(rotation.z, 360.0f);

    auto program = shader->GetProgram();
    glUseProgram(program);

    GLint modelLoc = glGetUniformLocation(program, "u_modelMatrix");
    GLint normalLoc = glGetUniformLocation(program, "u_normalMatrix");

    mat4 modelMat = mat4::transform(position, rotation, scale);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMat.getData());
    glUniformMatrix4fv(normalLoc, 1, GL_FALSE, modelMat.inv().T().getData());
    mesh->Draw(shader);
    glUseProgram(0);
}

//くコ:彡