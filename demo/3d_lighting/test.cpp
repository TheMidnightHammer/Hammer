/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the GPL 3.0 license
 * LICENSE file in the root directory of this source tree.
 */

#include "../../include/HammerEngine/HammerEngine.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>


int main() {
    HammerEngine Engine;

    Engine.enableValidationLayers = true;
    Engine.WindowWidth = 1200;
    Engine.WindowHeight = 900;
    Engine.MaxTextures = 1000;
    Engine.mouseLock = 1; 
    Engine.windowName = "Hammer Engine - Model";
    Engine.renderDistance = 1000.0f;
    Engine.cameraSpeed = 0.1f;

    Engine.cameraPosition = glm::vec3(5.0f, 5.0f, 15.0f);
    Engine.cameraFront = glm::normalize(glm::vec3(0.0f, -0.5f, -1.0f));

    Engine.initWindow();
    Engine.initVulkan();

    std::string vPath = "shaders/vert.spv";
    std::string fPath = "shaders/frag.spv";
    

    struct Particle { 
        std::vector<int> particleList; 
    };

    Particle particles; 
    particles.particleList.push_back(10);
    particles.particleList.push_back(20);
    particles.particleList.push_back(30);

    // Calculate exact byte size of the vector payload
    VkDeviceSize bufferSize = particles.particleList.size() * sizeof(int);
    HammerSSBO* my_SSBO = new HammerSSBO(&Engine, particles.particleList.data(), bufferSize);
    
    // Allocate pipeline with new
    HammerPipeline* mainPipeline = new HammerPipeline(Engine, vPath, fPath, 1, true, my_SSBO);

    HammerModel model("model/teapot.obj");

    // Allocate texture with new
    HammerTexture* dirtTexture = new HammerTexture(Engine, "textures/texture.png", HammerTextureFilter::Nearest);

    // Allocate mesh with new and push the raw pointer to the engine
    HammerMesh* myMesh1 = new HammerMesh(Engine, mainPipeline, dirtTexture, model.vertexData, model.indexData);
    
    Engine.meshs.push_back(myMesh1);

    Engine.drawPassStart();
    while (!glfwWindowShouldClose(Engine.window)) {
        Engine.updateFrameTimeStart();

        Engine.updateCameraDefault3D();
        
        Engine.drawFrame(); 
        
        Engine.updateFrameTimeEnd();
    }
    Engine.drawPassEnd();

    // Clean up allocated resources
    delete mainPipeline;
    delete dirtTexture;
    delete my_SSBO;
    // myMesh is cleaned up inside Engine.cleanup() via the loop we added earlier

    Engine.cleanup();

    return EXIT_SUCCESS;
}