
#define GLEW_STATIC 1
#define STB_IMAGE_IMPLEMENTATION

#include <iomanip>
#include <iostream>
#include <math.h>
#include <string>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "GUI.h"
#include "Camera.h"
#include "Mesh.h"
#include "MeshOBJ.h"
#include "Skybox.h"
#include "SceneObject.h"
#include "LightSource.h"

#include "DragonData.h"

#include "utils/GLShader.h"
#include "utils/Matrix.h"
#include "utils/Vertex.h"

int windowWidth = 800, windowHeight = 600;
std::string windowTitle = "NablaGL";

float prevTime = 0.0f;
float deltaTime = 0.0f;

bool mouseCaptured = false;
float lastMouseX = -1.0f, lastMouseY = -1.0f;

/* Scene contents */
Camera camera;
Skybox skybox;
LightSource light;

std::map<std::string,GLShader*> shaders;
std::map<GLShader*,std::string> shadersLookup; /* Reverse shader map to get shader name from pointer */

std::map<std::string,Mesh*> meshes;
std::map<Mesh*,std::string> meshesLookup;

std::map<std::string,SceneObject> objects;

void AddShader(const std::string& name,
               const std::string& vsPath,
               const std::string& fsPath,
               const std::string& gsPath = "") {
    if(shaders.find(name) != shaders.end()) {
        std::cout << "Shader \"" << name << "\" already loaded\n";
        return;
    }

    shaders.insert(std::make_pair(name, new GLShader()));
    GLShader* s = shaders.at(name);
    s->LoadVertexShader(vsPath.c_str());
    s->LoadFragmentShader(fsPath.c_str());
    if(!gsPath.empty())
        s->LoadGeometryShader(gsPath.c_str());
    s->Create();
    shadersLookup.insert(std::make_pair(s,name));
}

void AddMesh(const std::string &name, const std::vector<float> &vertices,
             const std::vector<unsigned int> &indices,
             const std::string &texture) {
    if (meshes.find(name) != meshes.end()) {
        std::cout << "Mesh \"" << name << "\" already loaded\n";
        return;
    }

    meshes.insert(std::make_pair(name, new Mesh()));
    Mesh *m = meshes.at(name);
    m->LoadMesh(vertices, indices, texture, skybox.GetTID());
    meshesLookup.insert(std::make_pair(m,name));
}


void AddModel(const std::string &name, const std::string &path) {
    if (meshes.find(name) != meshes.end()) {
        std::cout << "Model \"" << name << "\" already loaded\n";
        return;
    }
    meshes.insert(std::make_pair(name, new MeshOBJ()));
    MeshOBJ *m = static_cast<MeshOBJ*>(meshes.at(name));
    m->LoadFile(path);
    meshesLookup.insert(std::make_pair(m,name));
}

void AddObject(const std::string& name,
               const std::string& mesh,
               const std::string& shader,
               const vec3& position = {0.0f, 0.0f, 0.0f},
               const vec3& rotation = {0.0f, 0.0f, 0.0f},
               const vec3& scale = {1.0f, 1.0f, 1.0f}) {
    
    if(objects.find(name) != objects.end()) {
        std::cout << "Object \"" << name << "\" already loaded\n";
        return;
    }
    
    objects.insert(std::make_pair(name, SceneObject(
        meshes.at(mesh),
        shaders.at(shader),
        position,
        rotation,
        scale
    )));
}

bool Initialise() {
    GLenum ret = glewInit();

    camera = Camera({1.5f, 0.0f, 7.5f},
                    {-90.0f, 0.0f},
                    45.0f, (float)windowWidth/(float)windowHeight, 0.1f, 100.0f,
                    0.075f, 0.75f);

    skybox.Init({"assets/Skybox/px.png",
                 "assets/Skybox/nx.png",
                 "assets/Skybox/py.png",
                 "assets/Skybox/ny.png",
                 "assets/Skybox/pz.png",
                 "assets/Skybox/nz.png"});

    light.Init();
    light.GetPosition() = {10.5f, 1.5f, 5.0f};
    
    AddMesh("dragon", DragonVertices, DragonIndices, "assets/Textures/dragon.png");
    AddModel("level", "assets/Models/Level/WF.obj");
    AddModel("enemy", "assets/Models/Enemy/thwomp.obj");
    AddModel("star", "assets/Models/Star/star.obj");
    
    AddModel("text_obj_importation", "assets/Models/Text/obj_importation.obj");
    AddModel("text_texture_coordinates", "assets/Models/Text/texture_coordinates.obj");
    AddModel("text_unlit", "assets/Models/Text/unlit.obj");
    AddModel("text_lambert_shading", "assets/Models/Text/lambert_shading.obj");
    AddModel("text_phong_shading", "assets/Models/Text/phong_shading.obj");
    AddModel("text_blinn_phong_shading", "assets/Models/Text/blinn_phong_shading.obj");
    AddModel("text_environment_mapping", "assets/Models/Text/environment_mapping.obj");

    /* Load shaders */
    AddShader("unlit", "assets/Shaders/model.vs", "assets/Shaders/unlit.fs");
    AddShader("texcoord", "assets/Shaders/model.vs", "assets/Shaders/texcoord.fs");
    AddShader("lambert", "assets/Shaders/model.vs", "assets/Shaders/lambert.fs");
    AddShader("phong", "assets/Shaders/model.vs", "assets/Shaders/phong.fs");
    AddShader("blinn_phong", "assets/Shaders/model.vs", "assets/Shaders/blinn_phong.fs");
    AddShader("envmap", "assets/Shaders/model.vs", "assets/Shaders/envmap.fs");

    /* OBJ importation & model matrix */
    AddObject("Level", "level", "phong",
              { 0.5f,   1.0f, -3.0f},
              { 0.0f, -30.0f,  0.0f},
              {10.0f,  10.0f, 10.0f});
    AddObject("Enemy #1", "enemy", "unlit",
              {4.54f, 0.1f, -5.28f},
              {0.0f, 55.0f, 0.0f},
              {0.5f, 0.5f, 0.5f});
    AddObject("Enemy #2", "enemy", "unlit",
              {3.72f, 1.140f, -3.72f},
              {0.0f, 145.0f, 0.0f},
              {0.5f, 0.5f, 0.5f});
    AddObject("Star", "star", "unlit",
              {0.36f, 4.02f, -3.94f},
              {0.0f, 0.0f, 0.0f},
              {0.15f, 0.15f, 0.15f});
    AddObject("Text: OBJ importation", "text_obj_importation", "blinn_phong",
              {1.5f, -0.6f, 3.0f}, {0.0f, 0.0f, 0.0f}, {0.4f, 0.4f, 0.4f});

    /* Texture coordinates */
    AddObject("Dragon #1", "dragon", "texcoord", {6.0f, 0.0f, 3.0f},
              {0.0f, 0.0f, 0.0f}, {0.07f, 0.07f, 0.07f});
    AddObject("Text: Texture coordinates", "text_texture_coordinates", "blinn_phong",
              {6.0f, -0.25, 3.0f}, {0.0f, 0.0f, 0.0f}, {0.2f, 0.2f, 0.2f});

    /* Unlit */
    AddObject("Dragon #2", "dragon", "unlit", {7.5f, 0.0f, 3.0f},
              {0.0f, 0.0f, 0.0f}, {0.07f, 0.07f, 0.07f});
    AddObject("Text: Unlit", "text_unlit", "blinn_phong",
              {7.5f, -0.25, 3.0f}, {0.0f, 0.0f, 0.0f}, {0.2f, 0.2f, 0.2f});

    /* Lambert */
    AddObject("Dragon #3", "dragon", "lambert", {9.0f, 0.0f, 3.0f},
              {0.0f, 0.0f, 0.0f}, {0.07f, 0.07f, 0.07f});
    AddObject("Text: Lambert", "text_lambert_shading", "blinn_phong",
              {9.0f, -0.25, 3.0f}, {0.0f, 0.0f, 0.0f}, {0.2f, 0.2f, 0.2f});

    /* Phong */
    AddObject("Dragon #4", "dragon", "phong", {10.5f, 0.0f, 3.0f},
              {0.0f, 0.0f, 0.0f}, {0.07f, 0.07f, 0.07f});
    AddObject("Text: Phong", "text_phong_shading", "blinn_phong",
              {10.5f, -0.25, 3.0f}, {0.0f, 0.0f, 0.0f}, {0.2f, 0.2f, 0.2f});

    /* Blinn-Phong */
    AddObject("Dragon #5", "dragon", "blinn_phong", {12.0f, 0.0f, 3.0f},
              {0.0f, 0.0f, 0.0f}, {0.07f, 0.07f, 0.07f});
    AddObject("Text: Blinn-Phong", "text_blinn_phong_shading", "blinn_phong",
              {12.0f, -0.25, 3.0f}, {0.0f, 0.0f, 0.0f}, {0.2f, 0.2f, 0.2f});

    /* Envmap */
    AddObject("Dragon #6", "dragon", "envmap", {13.5f, 0.0f, 3.0f},
              {0.0f, 0.0f, 0.0f}, {0.07f, 0.07f, 0.07f});
    AddObject("Text: Envmap", "text_environment_mapping", "blinn_phong",
              {13.5f, -0.25, 3.0f}, {0.0f, 0.0f, 0.0f}, {0.2f, 0.2f, 0.2f});

    return true;
}

void Terminate() {
    for (auto& mesh : meshes) {
        delete mesh.second;
    }

    for (auto& shader : shaders) {
        shader.second->Destroy();
        delete shader.second;
    }
}

void Render(GLFWwindow *window) {
    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GUIBeginFrame();

    /* Bind view and projection matrices to all shaders with an UBO */
    auto cameraMatrices = camera.GetCameraMatrices();
    glBindBuffer(GL_UNIFORM_BUFFER, GLShader::GetCameraUBO());
    glBufferData(GL_UNIFORM_BUFFER, sizeof(cameraMatrices), &cameraMatrices, GL_STREAM_DRAW);

    /* Send light data w/ UBO */
    auto lightData = light.GetLightData();
    glBindBuffer(GL_UNIFORM_BUFFER, GLShader::GetLightUBO());
    glBufferData(GL_UNIFORM_BUFFER, sizeof(lightData), &lightData, GL_STREAM_DRAW);

    /* Draw skybox */
    skybox.Draw();

    /* Draw objects */
    for(auto& object : objects) {
        object.second.Draw();
    }

    light.DrawIndicator(camera);

    GUIDrawFrame(windowWidth, windowHeight,
                 camera, light,
                 shaders, shadersLookup,
                 meshes, meshesLookup,
                 objects);
}

/* Avoid calling GLFW each frame to get window size, only update it when it's
 * resized */
void OnWindowResize(GLFWwindow *window, int width, int height) {
    std::cout << "Resized: " << width << "x" << height << "\n";
    windowWidth = width;
    windowHeight = height;
    camera.GetRatio() = (float)windowWidth / (float)windowHeight;
}

void OnMouseClick(GLFWwindow *window, int button, int action, int mods) {
    /* Capture mouse on right click */
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouseCaptured = true;
        }
        if (action == GLFW_RELEASE) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseCaptured = false;
            lastMouseX = -1.0f;
            lastMouseY = -1.0f;
        }
    }
}

void ProcessInputs(GLFWwindow *window) {
    vec2 deltaMouse{0.0f, 0.0f};
    if (mouseCaptured) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        /* Avoid teleporting the camera when just capturing */
        if (lastMouseX != -1.0f && lastMouseY != -1.0f) {
            deltaMouse.x = (float)xpos - lastMouseX;
            deltaMouse.y = (float)ypos - lastMouseY;
        }

        lastMouseX = (float)xpos;
        lastMouseY = (float)ypos;
    }

    /* Keys are scancodes, so if on an AZERTY, Z = W */
    camera.Update(deltaTime,
                  glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS, /* up */
                  glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS, /* down */
                  glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS, /* left */
                  glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS, /* right */
                  glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS, /* shift */
                  deltaMouse);
}

void Update() {
    static float lastTime = 0;
    float currentTime = (float)glfwGetTime();

    objects.at("Star").Rotate({0.0f, deltaTime * 120.0f, 0.0f});
    objects.at("Enemy #1").Translate({0.0f, (sinf(2.0f * currentTime) - sinf(2.0f * lastTime)) / 5.0f, 0.0f});
    objects.at("Enemy #2").Translate({0.0f, (cosf(2.0f * currentTime) - cosf(2.0f * lastTime)) / 5.0f, 0.0f});

    for(int i = 1; i <= 6; i++)
        objects.at("Dragon #" + std::to_string(i)).Rotate({0.0f, deltaTime * 30.0f, 0.0f});


    lastTime = currentTime;
}

int main(void) {
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Anti-aliasing */
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(),
                              NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    // glfwSwapInterval(0); // <- VSYNC
    glfwSetWindowSizeCallback(window, OnWindowResize);
    glfwSetMouseButtonCallback(window, OnMouseClick);
    Initialise();
    GUIInitialise(window);

    /* Prevent triangles from overlapping */
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE); /* Doesn't work well with this model */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* SRGB */
    glEnable(GL_FRAMEBUFFER_SRGB);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        float currentTime = (float)glfwGetTime();
        deltaTime = currentTime - prevTime;
        prevTime = currentTime;

        /* Poll for and process events */
        glfwPollEvents();

        /* Handle input: keyboard and mouse */
        ProcessInputs(window);

        /* Update object positions */
        Update();

        /* Render here */
        Render(window);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    GUITerminate();
    Terminate();

    glfwTerminate();
    return 0;
}

//くコ:彡