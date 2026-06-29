/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the GPL 3.0 license
 * LICENSE file in the root directory of this source tree.
 */

#include "../include/HammerEngine/HammerEngine.h"
#include <iostream>
#include <vulkan/vulkan_core.h>

using namespace std;

void HammerEngine::runTest() {
    initWindow();
    initVulkan();
    mainLoopTest();
    cleanup();
}

void HammerEngine::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if(fullscreen){
        targetMonitor = glfwGetPrimaryMonitor(); 
    } else {
        targetMonitor = nullptr; 
    }
    
    window = glfwCreateWindow(WindowWidth, WindowHeight, windowName.c_str(), targetMonitor, nullptr);
    
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, HammerEngine::framebufferResizeCallback);

    if (mouseLock) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    
    // glfwMakeContextCurrent(window) this is a opengl command and i was calling this in vulkan, yikks
}

void HammerEngine::drawPassEnd(){
    vkDeviceWaitIdle(device);
}

void HammerEngine::mainLoopTest() {
    drawPassStart();

    while (!glfwWindowShouldClose(window)) {
        updateFrameTimeStart();

        updateCameraDefault3D();

        drawFrame();

        updateFrameTimeEnd();
    }

    drawPassEnd();
}

void HammerEngine::updateFrameTimeEnd(){
    auto end = std::chrono::high_resolution_clock::now();
    auto frameDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    totalFrameTime += frameDuration;

    if (currentTime - previousTime >= 1.0) {
        double averageFrameTime = totalFrameTime.count() / static_cast<double>(frameCount);

        FPS = frameCount;

        frameCount = 0;
        previousTime = currentTime;
        totalFrameTime = std::chrono::milliseconds(0);
    }
}

void HammerEngine::drawPassStart(){
    previousTime = glfwGetTime();
    frameCount = 0;

    std::chrono::milliseconds totalFrameTime(0);
}

void HammerEngine::updateFrameTimeStart(){
    start = std::chrono::high_resolution_clock::now();

    currentTime = glfwGetTime();
    frameCount++;
    glfwPollEvents();
}

void HammerEngine::setMaxVertciesIndicesSize(VkDeviceSize maxsize){
    stagingBufferSize = maxsize;
    stagingBuffer2Size = maxsize;
}