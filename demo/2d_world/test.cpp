/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the GPL 3.0 license
 * LICENSE file in the root directory of this source tree.
 */

#include "../../include/HammerEngine/HammerEngine.h"
#include "../../lib/imgui/imgui.h"
#include "../../lib/imgui/imgui_impl_glfw.h"
#include "../../lib/imgui/imgui_impl_vulkan.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>

int main() {
    HammerEngine Engine;

    Engine.enableValidationLayers = true;
    Engine.WindowWidth = 1000;
    Engine.WindowHeight = 1000;
    Engine.windowName = "Hammer Engine - 2D Texture Demo";
    Engine.MaxTextures = 1000;
    Engine.mouseLock = false;
    Engine.cameraSpeed = 0.1f;
    Engine.renderDistance = 16.0f;

    Engine.initWindow();
    Engine.initVulkan();
    Engine.InitImgui();

    std::string vPath = "shaders/vert.spv";
    std::string fPath = "shaders/frag.spv";
    
    // Converted to raw pointers
    HammerPipeline* mainPipeline = new HammerPipeline(
        Engine, vPath, fPath, 1, true, nullptr
    );

    HammerTexture* mainTexture = new HammerTexture(
        Engine, "textures/texture.png", HammerTextureFilter::Nearest
    );

    std::vector<Vertex> localVertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0000f, 0.0625f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0625f, 0.0625f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0625f, 0.0000f}, {1.0f, 0.0f, 0.0f}},
        {{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0.0000f, 0.0000f}, {1.0f, 0.0f, 0.0f}},
    };

    std::vector<uint32_t> localIndices = {
        0, 1, 2, 2, 3, 0
    };

    // Allocate the mesh with new and push the pointer
    HammerMesh* sceneMesh = new HammerMesh(
        Engine, 
        mainPipeline, 
        mainTexture, 
        localVertices, 
        localIndices
    );
    Engine.meshs.push_back(sceneMesh);

    // --- Main Loop ---
    Engine.drawPassStart();
    while (!glfwWindowShouldClose(Engine.window)) {
        Engine.updateFrameTimeStart();

        Engine.updateCameraDefault2D();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("hammer is framework now, blah blah blah");
        ImGui::Text("heheh do wahtever u want >:}");
        std::string temp_str = "FPS: " + std::to_string(Engine.FPS);
    
        const char* c_str = temp_str.c_str();

        ImGui::Text("%s", c_str);
        ImGui::End();
        

        ImGui::Render();
        
        Engine.drawFrame();

        Engine.updateFrameTimeEnd();
    }
    Engine.drawPassEnd();
    
    // Clean up allocated memory instead of .reset()
    delete mainTexture;
    delete mainPipeline;
    
    Engine.cleanup();

    return EXIT_SUCCESS;
}