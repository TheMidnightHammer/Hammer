/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the GPL 3.0 license
 * LICENSE file in the root directory of this source tree.
 */

#include "../../include/HammerEngine/HammerEngine.h"
#include "../../include/HammerEngine/HammerRect.h"
#include <glm/ext/vector_float3.hpp>
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
    Engine.windowWidth = 1000;
    Engine.windowHeight = 1000;
    Engine.MaxTextures = 1000;
    Engine.cameraPosition = glm::vec3(0, 0, 2);
    Engine.windowName = "Hammer Engine - Sprite Movement";
    Engine.mouseLock = false;
    Engine.cameraSpeed = 0.1f;
    Engine.renderDistance = 16.0f;
    Engine.setMaxBufferSize(1024*1024*16); // 16 mb of staging size

    Engine.initWindow();
    Engine.initVulkan();

    // Use raw pointers with the 'new' keyword
    HammerTexture* mainTexture = new HammerTexture(Engine, "textures/texture.png", HammerTextureFilter::Nearest);

    HammerRectSquareF player{0, 2, 1, 1};

    auto getVertices = [&](const HammerRectSquareF& p) {
        return std::vector<Vertex>{
            {{-0.5f, -0.5f, -1.5f}, {1.0f, 0.0f, 0.0f}, {0.0000f, 0.0625f}},
            {{ 0.5f, -0.5f, -1.5f}, {0.0f, 1.0f, 0.0f}, {0.0625f, 0.0625f}},
            {{ 0.5f,  0.5f, -1.5f}, {0.0f, 0.0f, 1.0f}, {0.0625f, 0.0000f}},
            {{-0.5f,  0.5f, -1.5f}, {1.0f, 1.0f, 0.0f}, {0.0000f, 0.0000f}}
        };
    };

    std::vector<uint32_t> localIndices = { 0, 1, 2, 2, 3, 0 };

    std::string vShader = "shaders/vert.spv";
    std::string fShader = "shaders/frag.spv";

    HammerPipeline* mainPipeline = new HammerPipeline(Engine, vShader, fShader, 1, true, NULL);
    
    HammerMesh* sceneMesh = new HammerMesh(Engine, mainPipeline, mainTexture, getVertices(player), localIndices);

    // Push the raw pointer directly
    Engine.meshs.push_back(sceneMesh);

    Engine.drawPassStart();
    while (!glfwWindowShouldClose(Engine.window)) {
        Engine.updateFrameTimeStart();

        bool moved = false;
        if (glfwGetKey(Engine.window, GLFW_KEY_I) == GLFW_PRESS) { player.y += 0.1f; moved = true; }
        if (glfwGetKey(Engine.window, GLFW_KEY_K) == GLFW_PRESS) { player.y -= 0.1f; moved = true; }
        if (glfwGetKey(Engine.window, GLFW_KEY_J) == GLFW_PRESS) { player.x -= 0.1f; moved = true; }
        if (glfwGetKey(Engine.window, GLFW_KEY_L) == GLFW_PRESS) { player.x += 0.1f; moved = true; }

        if (moved) {
            sceneMesh->position = glm::vec3(player.x, player.y, 0);
        }

        Engine.updateCameraDefault3D();
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