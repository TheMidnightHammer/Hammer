/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the MIT license
 * LICENSE file in the root directory of this source tree.
 */

#include "../../include/HammerEngine/HammerEngine.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>



int main() {
    HammerInfo info(true, 1000, 1000, "Hammer", 1000, false, 1.0f, 16.0f, 1024*1024*16);

    HammerEngine Engine(info);
    Engine.initWindow();
    Engine.initVulkan();

    Engine.cameraPosition = glm::vec3(5.0f, 5.0f, 15.0f);
    Engine.cameraFront = glm::normalize(glm::vec3(0.0f, -0.5f, -1.0f));

    std::string vPath = "shaders/vert.spv";
    std::string fPath = "shaders/frag.spv";
    

    struct Light { 
        // Use 0.0 - 1.0 range
        // Ambient: White at 20% intensity
        alignas(16) glm::vec4 ambientLightColor{1.0f, 1.0f, 1.0f, 0.2f};

        // Light Position
        alignas(16) glm::vec3 lightPosition{0.0f, 0.0f, 0.0f}; 
        
        // PADDING: This ensures lightColor starts on a 16-byte boundary
        float padding; 

        // Light Color: Pure white at full intensity
        alignas(16) glm::vec4 lightColor{1.0f, 1.0f, 1.0f, 1.0f};
    };

    Light light; 

    // Calculate exact byte size of the vector payload
    VkDeviceSize bufferSize = sizeof(Light);
    HammerSSBO* my_SSBO = new HammerSSBO(&Engine, &light, bufferSize);
    
    // Allocate pipeline with new
    HammerPipeline* mainPipeline = new HammerPipeline(Engine, vPath, fPath, 1, true, my_SSBO);

    HammerModel model("model/teapot.obj");

    // Allocate texture with new
    HammerTexture* dirtTexture = new HammerTexture(Engine, "textures/texture.png", HammerTextureFilter::Nearest);

    // Allocate mesh with new and push the raw pointer to the engine
    HammerMesh* myMesh1 = new HammerMesh(Engine, mainPipeline, dirtTexture, model.vertexData, model.indexData);
    
    Engine.meshs.push_back(myMesh1);

    HammerAsyncLogger logger;

    Engine.drawPassStart();
    while (!glfwWindowShouldClose(Engine.window)) {
        Engine.updateFrameTimeStart();

        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        
        float radius = 5.0f;
        light.lightPosition = glm::vec3(
            radius * cos(time), 
            7.0f,               // Keep height constant
            radius * sin(time)
        );

        my_SSBO->updateData(&light, bufferSize);


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