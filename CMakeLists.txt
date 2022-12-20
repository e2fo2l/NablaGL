cmake_minimum_required(VERSION 2.8.12)
project(NablaGL)

add_definitions("-std=c++11")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(nablagl # ImGUI sources
                       source/utils/imgui/imgui_impl_glfw.cpp
                       source/utils/imgui/imgui_impl_opengl3.cpp
                       # OpenGL utils -- TP
                       source/utils/GLShader.cpp
                       # Project sources
                       source/Main.cpp
                       source/utils/Vertex.cpp
                       source/utils/Matrix.cpp
                       source/Mesh.cpp
                       source/MeshOBJ.cpp
                       source/Camera.cpp
                       source/LightSource.cpp
                       source/Skybox.cpp
                       source/SceneObject.cpp
                       source/GUI.cpp)
target_link_libraries(nablagl ${CONAN_LIBS})