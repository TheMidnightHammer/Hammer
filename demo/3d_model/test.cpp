/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the MIT license
 * LICENSE file in the root directory of this source tree.
 */

#include "../../include/HammerEngine/HammerEngine.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>



int main() {
    HammerInfo info(true, 1000, 1000, "Hammer", 1000, false, 1.0f, 16.0f, 1024*1024*16);

    HammerEngine Engine(info);
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