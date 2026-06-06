/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the GPL 3.0 license
 * LICENSE file in the root directory of this source tree.
 */

#include "../../include/HammerEngine/HammerEngine.h"
#include <GLFW/glfw3.h>
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
    Engine.mouseLock = 0;
    Engine.cameraSpeed = 0.1f;
    Engine.renderDistance = 16.0f;

    Engine.initWindow();
    Engine.initVulkan();

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
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0000f, 0.0625f}},
        {{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0625f, 0.0625f}},
        {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0625f, 0.0000f}},
        {{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0.0000f, 0.0000f}},
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

    HammerMesh* sceneMesh2 = new HammerMesh(
        Engine, 
        mainPipeline, 
        mainTexture, 
        localVertices, 
        localIndices
    );

    Engine.meshs.push_back(sceneMesh);
    Engine.meshs.push_back(sceneMesh2);

    sceneMesh2->position.x = 2;
    Engine.cameraPosition.z = 5;

    // --- Main Loop ---
    Engine.drawPassStart();
    while (!glfwWindowShouldClose(Engine.window)) {
        Engine.updateFrameTimeStart();

        if(glfwGetKey(Engine.window, GLFW_KEY_I)){
            sceneMesh2->draw = true;
        }
        if(glfwGetKey(Engine.window, GLFW_KEY_O)){
            sceneMesh2->draw = false;
        }

        Engine.updateCameraDefault2D();
        
        Engine.drawFrame();

        Engine.updateFrameTimeEnd();
    }
    Engine.drawPassEnd();
    
    delete mainTexture;
    delete mainPipeline;
    
    Engine.cleanup();

    return EXIT_SUCCESS;
}