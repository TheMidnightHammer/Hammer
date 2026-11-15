/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the MIT license
 * LICENSE file in the root directory of this source tree.
 */

#include "../include/Hammer/Hammer.h"
#include <iostream>
#include <vulkan/vulkan_core.h>

using namespace std;

void Hammer::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if(isFullscreen){
        targetMonitor = glfwGetPrimaryMonitor(); 
    } else {
        targetMonitor = nullptr; 
    }
    
    window = glfwCreateWindow(windowWidth, windowHeight, windowName.c_str(), targetMonitor, nullptr);
    
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, Hammer::framebufferResizeCallback);

    if (mouseLock) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    
    // glfwMakeContextCurrent(window) this is a opengl command and i was calling this in vulkan, yikks
}

void Hammer::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<Hammer*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

void Hammer::drawPassEnd(){
    vkDeviceWaitIdle(device);
}

void Hammer::updateFrameTimeEnd(){
    auto end = std::chrono::high_resolution_clock::now();
    auto frameDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    totalFrameTime += frameDuration;

    if (currentTime - previousTime >= 1.0) {
        FPS = frameCount;

        frameCount = 0;
        previousTime = currentTime;
        totalFrameTime = std::chrono::milliseconds(0);
    }
}

void Hammer::drawPassStart(){
    previousTime = glfwGetTime();
    frameCount = 0;

    std::chrono::milliseconds totalFrameTime(0);
}

void Hammer::updateFrameTimeStart(){
    start = std::chrono::high_resolution_clock::now();

    currentTime = glfwGetTime();
    frameCount++;
    glfwPollEvents();
}

