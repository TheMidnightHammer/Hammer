/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the GPL 3.0 license
 * LICENSE file in the root directory of this source tree.
 */

#include "../../include/HammerEngine/HammerEngine.h"
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
    HammerEngine engine;

    engine.enableValidationLayers = true;
    engine.windowWidth = 1000;
    engine.windowHeight = 1000;
    engine.windowName = "Hammer engine - 2D Texture Demo";
    engine.MaxTextures = 1000;
    engine.mouseLock = false;
    engine.cameraSpeed = 0.1f;
    engine.renderDistance = 16.0f;
    engine.setMaxBufferSize(1024*1024*16); // 16 mb of staging size

    engine.initWindow();
    engine.initVulkan();

    std::string vertShader = "shaders/vert.spv";
    std::string fragShader = "shaders/frag.spv";
    HammerPipeline* textPipeline = new HammerPipeline(engine, vertShader, fragShader, 1, true, nullptr);

    HammerFont font(engine, "fonts/Arial.ttf");

    HammerCustomTexture* helloWorldTex = font.createTextPtr(engine, "wow, facy text ? -TheMidnightHammer", 0, 48, 2048, 256, HammerTextureFilter::Linear);
    //                                                                                 X     Y    W     H
    HammerMesh* textMesh = engine.CreateTextQuad(engine, textPipeline, helloWorldTex, 0.0f, 0.0f, 8.0f, 2.0f);

    engine.addMeshRenderer(textMesh);

    engine.drawPassStart();
    while (!glfwWindowShouldClose(engine.window)) {
        engine.updateFrameTimeStart();
        
        engine.drawFrame();

        engine.updateCameraDefault3D();

        engine.updateFrameTimeEnd();
    }
    engine.drawPassEnd();
    
    // Clean up allocated memory instead of .reset()
    delete helloWorldTex;
    delete textPipeline;
    
    engine.cleanup();

    return EXIT_SUCCESS;
}