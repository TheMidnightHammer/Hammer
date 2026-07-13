/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the GPL 3.0 license
 * LICENSE file in the root directory of this source tree.
 */

#include "../../include/HammerEngine/HammerEngine.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>

std::array<VkVertexInputAttributeDescription, 4> Vertex::getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

    // 0: Position
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    // 1: Color
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    // 2: TexCoord
    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

    // 3: Normal
    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(Vertex, normal);

    return attributeDescriptions;
}


int main() {
    HammerEngine Engine;

    Engine.enableValidationLayers = true;
    Engine.windowWidth = 1200;
    Engine.windowHeight = 900;
    Engine.MaxTextures = 1000;
    Engine.mouseLock = true; 
    Engine.windowName = "Hammer Engine - Model";
    Engine.renderDistance = 1000.0f;
    Engine.cameraSpeed = 1.0f;
    Engine.setMaxBufferSize(1024*1024*16); // 16 mb of staging size

    Engine.cameraPosition = glm::vec3(5.0f, 5.0f, 15.0f);
    Engine.cameraFront = glm::normalize(glm::vec3(0.0f, -0.5f, -1.0f));

    Engine.initWindow();
    Engine.initVulkan();

    std::string vPath = "shaders/vert.spv";
    std::string fPath = "shaders/frag.spv";
    
    // Allocate pipeline with new
    HammerPipeline* mainPipeline = new HammerPipeline(Engine, vPath, fPath, 1, true, nullptr);

    HammerModel model("model/teapot.obj");

    // Allocate texture with new
    HammerTexture* dirtTexture = new HammerTexture(Engine, "textures/texture.png", HammerTextureFilter::Nearest);

    // Allocate mesh with new and push the raw pointer to the engine
    HammerMesh* myMesh = new HammerMesh(Engine, mainPipeline, dirtTexture, model.vertexData, model.indexData);
    
    Engine.meshs.push_back(myMesh);

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
    // myMesh is cleaned up inside Engine.cleanup() via the loop we added earlier

    Engine.cleanup();

    return EXIT_SUCCESS;
}