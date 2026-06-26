/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the GPL 3.0 license
 * LICENSE file in the root directory of this source tree.
 */

#include "../../include/HammerEngine/HammerEngine.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>

static const Vertex faceVertices[6][4] = {
    // Top face (+Y)
    {
        {{-0.5f, 0.5f,-0.5f},{1.0f,0.0f,0.0f},{0.1250f,0.0625f},{1.0f,0.0f,0.0f}},
        {{ 0.5f, 0.5f,-0.5f},{0.0f,1.0f,0.0f},{0.1875f,0.0625f},{1.0f,0.0f,0.0f}},
        {{ 0.5f, 0.5f, 0.5f},{0.0f,0.0f,1.0f},{0.1875f,0.0000f},{1.0f,0.0f,0.0f}},
        {{-0.5f, 0.5f, 0.5f},{1.0f,1.0f,0.0f},{0.1250f,0.0000f},{1.0f,0.0f,0.0f}}
    },
    // Bottom face (-Y)
    {
        {{-0.5f,-0.5f,-0.5f},{1.0f,0.0f,1.0f},{0.0625f,0.0625f},{1.0f,0.0f,0.0f}},
        {{ 0.5f,-0.5f,-0.5f},{0.0f,1.0f,1.0f},{0.1250f,0.0625f},{1.0f,0.0f,0.0f}},
        {{ 0.5f,-0.5f, 0.5f},{0.5f,0.5f,0.5f},{0.1250f,0.0000f},{1.0f,0.0f,0.0f}},
        {{-0.5f,-0.5f, 0.5f},{0.0f,0.0f,0.0f},{0.0625f,0.0000f},{1.0f,0.0f,0.0f}}
    },
    // Right face (+X)
    {
        {{ 0.5f,-0.5f,-0.5f},{1.0f,0.5f,0.0f},{0.0625f,0.0625f},{1.0f,0.0f,0.0f}},
        {{ 0.5f, 0.5f,-0.5f},{0.5f,1.0f,0.0f},{0.0625f,0.0000f},{1.0f,0.0f,0.0f}},
        {{ 0.5f, 0.5f, 0.5f},{0.0f,1.0f,0.5f},{0.0000f,0.0000f},{1.0f,0.0f,0.0f}},
        {{ 0.5f,-0.5f, 0.5f},{0.5f,0.0f,1.0f},{0.0000f,0.0625f},{1.0f,0.0f,0.0f}}
    },
    // Left face (-X)
    {
        {{-0.5f,-0.5f,-0.5f},{0.5f,0.0f,0.5f},{0.0625f,0.0625f},{1.0f,0.0f,0.0f}},
        {{-0.5f, 0.5f,-0.5f},{0.5f,0.5f,0.0f},{0.0625f,0.0000f},{1.0f,0.0f,0.0f}},
        {{-0.5f, 0.5f, 0.5f},{0.0f,0.5f,0.5f},{0.0000f,0.0000f},{1.0f,0.0f,0.0f}},
        {{-0.5f,-0.5f, 0.5f},{0.0f,0.0f,0.5f},{0.0000f,0.0625f},{1.0f,0.0f,0.0f}}
    },
    // Front face (+Z)
    {
        {{-0.5f,-0.5f, 0.5f},{1.0f,0.0f,0.0f},{0.0000f,0.0625f},{1.0f,0.0f,0.0f}},
        {{ 0.5f,-0.5f, 0.5f},{0.0f,1.0f,0.0f},{0.0625f,0.0625f},{1.0f,0.0f,0.0f}},
        {{ 0.5f, 0.5f, 0.5f},{0.0f,0.0f,1.0f},{0.0625f,0.0000f},{1.0f,0.0f,0.0f}},
        {{-0.5f, 0.5f, 0.5f},{1.0f,1.0f,0.0f},{0.0000f,0.0000f},{1.0f,0.0f,0.0f}}
    },
    // Back face (-Z)
    {
        {{-0.5f,-0.5f,-0.5f},{1.0f,0.0f,1.0f},{0.0000f,0.0625f},{1.0f,0.0f,0.0f}},
        {{ 0.5f,-0.5f,-0.5f},{0.0f,1.0f,1.0f},{0.0625f,0.0625f},{1.0f,0.0f,0.0f}},
        {{ 0.5f, 0.5f,-0.5f},{0.5f,0.5f,0.5f},{0.0625f,0.0000f},{1.0f,0.0f,0.0f}},
        {{-0.5f, 0.5f,-0.5f},{0.0f,0.0f,0.0f},{0.0000f,0.0000f},{1.0f,0.0f,0.0f}}
    }
};

static const uint32_t faceIndicesPattern[] = { 0, 1, 2, 2, 3, 0 };

void generateCubeGrid(std::vector<Vertex>* outVertices, std::vector<uint32_t>* outIndices, int width, int height, int depth) {
    uint32_t vertexOffset = 0;
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            for (int z = 0; z < depth; ++z) {
                glm::vec3 worldPos = glm::vec3((float)x, (float)y, (float)z);
                for (int face = 0; face < 6; ++face) {
                    for (int i = 0; i < 4; ++i) {
                        Vertex v = faceVertices[face][i];
                        v.pos += worldPos;
                        outVertices->push_back(v);
                    }
                    for (int i = 0; i < 6; ++i) {
                        outIndices->push_back(faceIndicesPattern[i] + vertexOffset);
                    }
                    vertexOffset += 4;
                }
            }
        }
    }
}

int main() {
    HammerEngine Engine;

    Engine.enableValidationLayers = true;
    Engine.WindowWidth = 1200;
    Engine.WindowHeight = 900;
    Engine.MaxTextures = 1000;
    Engine.mouseLock = 1; 
    Engine.windowName = "Hammer Engine - Multi Texture";
    Engine.renderDistance = 1000.0f;
    Engine.cameraSpeed = 1.0f;

    Engine.cameraPosition = glm::vec3(5.0f, 5.0f, 15.0f);
    Engine.cameraFront = glm::normalize(glm::vec3(0.0f, -0.5f, -1.0f));

    Engine.initWindow();
    Engine.initVulkan();

    std::string vPath = "shaders/vert.spv";
    std::string fPath = "shaders/frag.spv";
    
    // Allocate raw pointer for pipeline
    HammerPipeline* mainPipeline = new HammerPipeline(Engine, vPath, fPath, 1, true, nullptr);

    // Allocate raw pointer for texture
    HammerTexture* dirtTexture = new HammerTexture(Engine, "textures/texture.png", HammerTextureFilter::Nearest);

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    generateCubeGrid(&vertices, &indices, 10, 10, 10);

    // Allocate raw pointer for mesh and add to engine vector
    HammerMesh* myMesh = new HammerMesh(Engine, mainPipeline, dirtTexture, vertices, indices);
    Engine.meshs.push_back(myMesh);

    Engine.drawPassStart();
    while (!glfwWindowShouldClose(Engine.window)) {
        Engine.updateFrameTimeStart();

        Engine.updateCameraDefault3D();
        
        Engine.drawFrame(); 
        
        Engine.updateFrameTimeEnd();
    }
    Engine.drawPassEnd();

    // Clean up allocated memory
    delete mainPipeline;
    delete dirtTexture;
    
    // Engine.cleanup() will delete the pointers inside Engine.meshs
    Engine.cleanup();

    return EXIT_SUCCESS;
}