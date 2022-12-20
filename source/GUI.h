#pragma once

#include <map>
#include <GLFW/glfw3.h>

#include "utils/GLShader.h"
#include "Camera.h"
#include "Mesh.h"
#include "LightSource.h"
#include "SceneObject.h"

void GUIInitialise(GLFWwindow* window);
void GUITerminate();

void GUIBeginFrame();
void GUIDrawFrame(int width, int height,
                  Camera& camera,
                  LightSource& light,
                  std::map<std::string,GLShader*>& shaders,
                  std::map<GLShader*,std::string>& shadersLookup,
                  std::map<std::string,Mesh*>& meshes,
                  std::map<Mesh*,std::string>& meshesLookup,
                  std::map<std::string,SceneObject>& objects);

//くコ:彡