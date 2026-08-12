/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the MIT license
 * LICENSE file in the root directory of this source tree.
 */

#include "../../include/HammerEngine/HammerEngine.h"
#include <vector>
#include <string>
#include <cmath>
#include <glm/glm.hpp>

static const Vertex faceVertices[6][4] = {
    // Top face (+Y)
    {{{-0.5f, 0.5f,-0.5f},{1.0f,0.0f,0.0f},{0.1250f,0.0625f}}, {{ 0.5f, 0.5f,-0.5f},{0.0f,1.0f,0.0f},{0.1875f,0.0625f}}, {{ 0.5f, 0.5f, 0.5f},{0.0f,0.0f,1.0f},{0.1875f,0.0000f}}, {{-0.5f, 0.5f, 0.5f},{1.0f,1.0f,0.0f},{0.1250f,0.0000f}}},
    // Bottom face (-Y)
    {{{-0.5f,-0.5f,-0.5f},{1.0f,0.0f,1.0f},{0.0625f,0.0625f}}, {{ 0.5f,-0.5f,-0.5f},{0.0f,1.0f,1.0f},{0.1250f,0.0625f}}, {{ 0.5f,-0.5f, 0.5f},{0.5f,0.5f,0.5f},{0.1250f,0.0000f}}, {{-0.5f,-0.5f, 0.5f},{0.0f,0.0f,0.0f},{0.0625f,0.0000f}}},
    // Right face (+X)
    {{{ 0.5f,-0.5f,-0.5f},{1.0f,0.5f,0.0f},{0.0625f,0.0625f}}, {{ 0.5f, 0.5f,-0.5f},{0.5f,1.0f,0.0f},{0.0625f,0.0000f}}, {{ 0.5f, 0.5f, 0.5f},{0.0f,1.0f,0.5f},{0.0000f,0.0000f}}, {{ 0.5f,-0.5f, 0.5f},{0.5f,0.0f,1.0f},{0.0000f,0.0625f}}},
    // Left face (-X)
    {{{-0.5f,-0.5f,-0.5f},{0.5f,0.0f,0.5f},{0.0625f,0.0625f}}, {{-0.5f, 0.5f,-0.5f},{0.5f,0.5f,0.0f},{0.0625f,0.0000f}}, {{-0.5f, 0.5f, 0.5f},{0.0f,0.5f,0.5f},{0.0000f,0.0000f}}, {{-0.5f,-0.5f, 0.5f},{0.0f,0.0f,0.5f},{0.0000f,0.0625f}}},
    // Front face (+Z)
    {{{-0.5f,-0.5f, 0.5f},{1.0f,0.0f,0.0f},{0.0000f,0.0625f}}, {{ 0.5f,-0.5f, 0.5f},{0.0f,1.0f,0.0f},{0.0625f,0.0625f}}, {{ 0.5f, 0.5f, 0.5f},{0.0f,0.0f,1.0f},{0.0625f,0.0000f}}, {{-0.5f, 0.5f, 0.5f},{1.0f,1.0f,0.0f},{0.0000f,0.0000f}}},
    // Back face (-Z)
    {{{-0.5f,-0.5f,-0.5f},{1.0f,0.0f,1.0f},{0.0000f,0.0625f}}, {{ 0.5f,-0.5f,-0.5f},{0.0f,1.0f,1.0f},{0.0625f,0.0625f}}, {{ 0.5f, 0.5f,-0.5f},{0.5f,0.5f,0.5f},{0.0625f,0.0000f}}, {{-0.5f, 0.5f,-0.5f},{0.0f,0.0f,0.0f},{0.0000f,0.0000f}}}
};

static const uint32_t faceIndicesPattern[] = { 0, 1, 2, 2, 3, 0 };

void generateCubeGrid(std::vector<Vertex>& outVertices, std::vector<uint32_t>& outIndices, int width, int height, int depth) {
    uint32_t vertexOffset = 0;
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            for (int z = 0; z < depth; ++z) {
                glm::vec3 worldPos = glm::vec3((float)x, (float)y, (float)z);
                for (int face = 0; face < 6; ++face) {
                    for (int i = 0; i < 4; ++i) {
                        Vertex v = faceVertices[face][i];
                        v.pos += worldPos;
                        outVertices.push_back(v);
                    }
                    for (int i = 0; i < 6; ++i) {
                        outIndices.push_back(faceIndicesPattern[i] + vertexOffset);
                    }
                    vertexOffset += 4;
                }
            }
        }
    }
}

int main() {
    HammerInfo info(true, 1000, 1000, "Hammer", 1000, false, 1.0f, 16.0f, 1024*1024*16);

    HammerEngine Engine(info);
    Engine.initWindow();
    Engine.initVulkan();

    std::string vPath = "shaders/vert.spv";
    std::string fPath = "shaders/frag.spv";
    
    // Allocate raw pointers
    HammerPipeline* mainPipeline = new HammerPipeline(Engine, vPath, fPath, 1, true, nullptr);
    HammerTexture* boxTexture = new HammerTexture(Engine, "textures/texture.png", HammerTextureFilter::Nearest);

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    generateCubeGrid(vertices, indices, 10, 10, 10);

    // Create and store mesh
    HammerMesh* myMesh = new HammerMesh(Engine, mainPipeline, boxTexture, vertices, indices);
    Engine.meshs.push_back(myMesh);

    myMesh->updateBuffers(vertices, indices);

    Engine.drawPassStart();
    while (!glfwWindowShouldClose(Engine.window)) {
        Engine.updateFrameTimeStart();

        float time = static_cast<float>(glfwGetTime());

        // Animate the mesh using the raw pointer
        myMesh->position.y = sin(time) * 0.5f; 
        myMesh->rotation.y = time * 20.0f;

        Engine.updateCameraDefault3D();
        
        Engine.drawFrame(); 
        
        Engine.updateFrameTimeEnd();
    }
    Engine.drawPassEnd();

    // Manual cleanup for pipeline and texture
    delete boxTexture;
    delete mainPipeline;

    // Engine.cleanup() will handle deleting all pointers in Engine.meshs
    Engine.cleanup();

    return EXIT_SUCCESS;
}
