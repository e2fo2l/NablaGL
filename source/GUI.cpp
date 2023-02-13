#include "GUI.h"

#include "utils/imgui/imgui_impl_glfw.h"
#include "utils/imgui/imgui_impl_opengl3.h"

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <imgui.h>
#include <string>

#define FRAME_KEEP_COUNT 200
float frameTimes[FRAME_KEEP_COUNT];

void GUIInitialise(GLFWwindow *window) {
    for (int i = 0; i < FRAME_KEEP_COUNT; i++)
        frameTimes[i] = 0.0f;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void GUITerminate() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUIBeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUIBeginCornerWindow(int corner = 0) {
    // https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp#L7310=
    const float PAD = 10.0f;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImVec2 work_pos =
        viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos, window_pos_pivot;
    window_pos.x =
        (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
    window_pos.y =
        (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
    window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
    window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    ImGui::SetNextWindowBgAlpha(0.6f); // Transparent background
    ImGui::Begin(
        std::string("corner overlay window" + std::to_string(corner)).c_str(),
        NULL, window_flags);
}

void GUIVec2Widget(const std::string& title, vec2& v,
                   const std::string& xLabel = "x",
                   const std::string& yLabel = "y") {
    if(!title.empty())
        ImGui::Text("%s", title.c_str());
    ImGui::PushItemWidth((0.65f * ImGui::GetWindowSize().x) / 3.0f);
    ImGui::DragFloat(xLabel.c_str(), &v.x);
    ImGui::SameLine();
    ImGui::DragFloat(yLabel.c_str(), &v.y);
    ImGui::PopItemWidth();
}

void GUIVec3Widget(const std::string& title, vec3& v,
                   const std::string& xLabel = "x",
                   const std::string& yLabel = "y",
                   const std::string& zLabel = "z",
                   float speed = 0.02f) {
    if(!title.empty())
        ImGui::Text("%s", title.c_str());
    ImGui::PushItemWidth((0.65f * ImGui::GetWindowSize().x) / 3.0f);
    ImGui::DragFloat(xLabel.c_str(), &v.x, speed);
    ImGui::SameLine();
    ImGui::DragFloat(yLabel.c_str(), &v.y, speed);
    ImGui::SameLine();
    ImGui::DragFloat(zLabel.c_str(), &v.z, speed);
    ImGui::PopItemWidth();
}

void GUIMat4Widget(const std::string& title,
                   const mat4& m) {
    if(!title.empty())
        ImGui::Text("%s", title.c_str());
    
    if (ImGui::BeginTable("##table", 4))
    {
        for (int i = 0; i < 4; i++)
        {
            ImGui::TableNextRow();
            for (int j = 0; j < 4; j++)
            {
                ImGui::TableSetColumnIndex(j);
                ImGui::Text("%.3f", m(i,j));
            }
        }
        ImGui::EndTable();
    }
}
void GUICameraWindow(Camera& camera) {
    ImGui::Begin("Camera");
    if(ImGui::CollapsingHeader("View")) {
        GUIVec3Widget("Position", camera.GetPosition());
        GUIVec2Widget("Rotation", camera.GetRotation(), "Yaw", "Pitch");
    }
    if(ImGui::CollapsingHeader("Projection")) {
        ImGui::Text("FOV"); ImGui::DragFloat("##label1", &camera.GetFOV(), 0.2f, 1.0, 180.0f);
        ImGui::Text("Ratio"); ImGui::DragFloat("##label2", &camera.GetRatio(), 0.2f, 0.1f, 3.0f);
        ImGui::Text("Near"); ImGui::DragFloat("##label3", &camera.GetNear(), 0.001f, 1e-6f, 100.0f);
        ImGui::Text("Far"); ImGui::DragFloat("##label4", &camera.GetFar(), 0.2f, 0.01f, 1e6f);
    }
    if(ImGui::CollapsingHeader("Settings")) {
        ImGui::Text("Speed"); ImGui::DragFloat("##label5", &camera.GetSpeed(), 0.02f, 0.0f, 100.0f);
        ImGui::Text("Sensivity"); ImGui::DragFloat("##label6", &camera.GetSensivity(), 0.002f, 0.001f, 0.2f);
    }
    if(ImGui::CollapsingHeader("Matrices")) {
        /* Matrices have to be recalculated there, could use some caching */
        GUIMat4Widget("View Matrix", camera.ViewMatrix());
        GUIMat4Widget("Projection Matrix", camera.ProjectionMatrix());
    }

    ImGui::End();
}

void GUILightWindow(LightSource& light) {
    ImGui::Begin("Light Source");
    GUIVec3Widget("Position", light.GetPosition());

    ImGui::Text("Ambient");  ImGui::DragFloat("##label7", &light.GetAmbient(), 0.0005f, 0.0f, 5.0f);
    ImGui::Text("Specular"); ImGui::DragFloat("##label8", &light.GetSpecular(), 0.0005f, 0.0f, 1.0f);
    
    float color[3] = { light.GetColor().x, light.GetColor().y, light.GetColor().z };
    ImGui::Text("Color"); ImGui::ColorPicker3("##label9", color);
    light.GetColor() = { color[0], color[1], color[2] };

    ImGui::End();
}

void GUIDrawFrame(int width, int height,
                  Camera& camera,
                  LightSource& light,
                  std::map<std::string,GLShader*>& shaders,
                  std::map<GLShader*,std::string>& shadersLookup,
                  std::map<std::string,Mesh*>& meshes,
                  std::map<Mesh*,std::string>& meshesLookup,
                  std::map<std::string,SceneObject>& objects) {

    /* Update frame time history */
    float avgFrametime = 0.0f;
    for (int i = 0; i < FRAME_KEEP_COUNT - 1; i++) {
        frameTimes[i] = frameTimes[i + 1];
        avgFrametime += frameTimes[i];
    }
    frameTimes[FRAME_KEEP_COUNT - 1] = 1.0f / ImGui::GetIO().DeltaTime;
    avgFrametime += frameTimes[FRAME_KEEP_COUNT - 1];
    avgFrametime /= (float)FRAME_KEEP_COUNT;

    /* Info window */
    std::string gpuRenderer((const char *)glGetString(GL_RENDERER));
    if (gpuRenderer.length() >= 20)
        gpuRenderer = gpuRenderer.substr(0, 17) + "...";
    GUIBeginCornerWindow(0);
    ImGui::Text("NablaGL");
    ImGui::Text("Made by Efflam O.");
    ImGui::Text("Built on %s %s", __DATE__, __TIME__);
    ImGui::Text("Renderer: %s", gpuRenderer.c_str());
    ImGui::Text("Resolution: %dx%d", width, height);
    ImGui::Text("FPS: %d", (int)(avgFrametime + 0.5f));
    ImGui::PlotLines("", frameTimes, FRAME_KEEP_COUNT, 0, NULL, 0, 150);
    ImGui::End();

    /* Controls window */
    GUIBeginCornerWindow(1);
    ImGui::Text("Controls");
    ImGui::Text("Move:  Z");
    ImGui::Text("     Q S D");
    ImGui::Text("Shift: Move faster");
    ImGui::Text("Right click + move");
    ImGui::Text("mouse to look around");
    ImGui::End();

    GUICameraWindow(camera);
    GUILightWindow(light);

    std::vector<const char*> shaderList;
    for(auto& shader : shaders) shaderList.push_back(shader.first.c_str());
    std::vector<const char*> meshList;
    for(auto& mesh : meshes) meshList.push_back(mesh.first.c_str());

    /* Objects window */
    ImGui::Begin("Objects");
    for(auto& object : objects) {
        if(ImGui::CollapsingHeader(object.first.c_str())) {
            std::string t = object.first;                         /* Hack to get different names for sliders */
            GUIVec3Widget("Position", object.second.GetPosition(), "x##p"+t, "y##p"+t, "z##p"+t);
            GUIVec3Widget("Rotation", object.second.GetRotation(), "x##r"+t, "y##r"+t, "z##r"+t, 1.0f);
            GUIVec3Widget("Scale", object.second.GetScale(),       "x##s"+t, "y##s"+t, "z##s"+t);

            int shaderPos = std::distance(shaders.begin(), shaders.find(shadersLookup.at(object.second.GetShader())));
            std::string shaderName = shadersLookup.at(object.second.GetShader());
            for(int i = 0; i < shaderList.size(); i++)
                if(shaderName == shaderList[i]) { shaderPos = i; break; }
            ImGui::Text("Shader");
            ImGui::Combo(("##shader" + t).c_str(), &shaderPos, shaderList.data(), shaderList.size());
            object.second.GetShader() = shaders.at(shaderList[shaderPos]);
            
            int meshPos = std::distance(meshes.begin(), meshes.find(meshesLookup.at(object.second.GetMesh())));
            ImGui::Text("Mesh");
            ImGui::Combo(("##mesh" + t).c_str(), &meshPos, meshList.data(), meshList.size());
            object.second.GetMesh() = meshes.at(meshList[meshPos]);
        }
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//くコ:彡