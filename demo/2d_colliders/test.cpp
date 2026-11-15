/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the MIT license
 * LICENSE file in the root directory of this source tree.
 */

#include "../../include/Hammer/Hammer.h"
#include "../../include/Hammer/HammerRect.h"
#include <glm/ext/vector_float3.hpp>
#include <vector>
#include <string>
#include <glm/glm.hpp>


int main() {
    HammerInfo info {
        .isFullscreen = false,
        .windowWidth = 1000,
        .windowHeight = 1000,
        .windowName = "Hammer",
        .maxTextures = 1000, // what ever you want really
        .mouseLock = false,
        .cameraSpeed = 1.0f,
        .renderDistance = 16.0f,
        .stagingBufferSize = 1024*1024*16,
    };

    Hammer* engine = new Hammer(info);
    engine->initWindow();
    engine->initVulkan();

    // Use raw pointers with the 'new' keyword
    HammerTexture* mainTexture = new HammerTexture(engine, "textures/texture.png", HammerTextureFilter::Nearest);

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

    HammerPipeline* mainPipeline = new HammerPipeline(engine, vShader, fShader, 1, true, NULL);
    
    HammerMesh* sceneMesh = new HammerMesh(engine, mainPipeline, mainTexture, getVertices(player), localIndices);

    // Push the raw pointer directly
    engine->meshes.push_back(sceneMesh);

    engine->drawPassStart();
    while (!glfwWindowShouldClose(engine->window)) {
        engine->updateFrameTimeStart();

        bool moved = false;
        if (glfwGetKey(engine->window, GLFW_KEY_I) == GLFW_PRESS) { player.y += 0.1f; moved = true; }
        if (glfwGetKey(engine->window, GLFW_KEY_K) == GLFW_PRESS) { player.y -= 0.1f; moved = true; }
        if (glfwGetKey(engine->window, GLFW_KEY_J) == GLFW_PRESS) { player.x -= 0.1f; moved = true; }
        if (glfwGetKey(engine->window, GLFW_KEY_L) == GLFW_PRESS) { player.x += 0.1f; moved = true; }

        if (moved) {
            sceneMesh->position = glm::vec3(player.x, player.y, 0);
        }

        engine->updateCameraDefault3D();
        engine->drawFrame();
        engine->updateFrameTimeEnd();
    }
    engine->drawPassEnd();

    // Clean up allocated memory instead of .reset()
    delete mainTexture;
    delete mainPipeline;
    
    engine->cleanup();

    delete engine;

    return EXIT_SUCCESS;
}