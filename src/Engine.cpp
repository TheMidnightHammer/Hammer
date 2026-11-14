/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the MIT license
 * LICENSE file in the root directory of this source tree.
 */

#include "EngineCommon.h"

#include "../lib/imgui/imgui.h"
#include "../lib/imgui/imgui_impl_glfw.h"
#include "../lib/imgui/imgui_impl_vulkan.h"

void HammerEngine::initVulkan() {
    createInstance();
#ifdef ENABLE_VULKAN_VALIDATION_LAYERS
    setupDebugMessenger();
#endif
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDescriptorSetLayout();
    createCommandPool();
    createDepthResources();
    createFramebuffers();
    createStagingBuffer();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
    createSyncObjects();
}

void HammerEngine::cleanup() {

    vkDeviceWaitIdle(device);

    for (HammerMesh* mesh : meshes) {
        delete mesh;
    }
    meshes.clear();

    if(useImGui){
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        vkDestroyDescriptorPool(device, imguiPool, nullptr);
    }

    cleanupSwapChain();

    vkDestroyRenderPass(device, renderPass, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(device, uniformBuffers[i], nullptr);
        vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
    }

    if (stagingBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, stagingBuffer, nullptr);
        stagingBuffer = VK_NULL_HANDLE;
    }
    if (stagingBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(device, stagingBufferMemory, nullptr);
        stagingBufferMemory = VK_NULL_HANDLE;
    }


    if (stagingBuffer2 != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, stagingBuffer2, nullptr);
        stagingBuffer = VK_NULL_HANDLE;
    }
    if (&vkBindBufferMemory2 != VK_NULL_HANDLE) {
        vkFreeMemory(device, stagingBuffer2Memory, nullptr);
        stagingBuffer2Memory = VK_NULL_HANDLE;
    }

    vkDestroyDescriptorPool(device, descriptorPool, nullptr);

    vkDestroyDescriptorSetLayout(device, globalSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(device, textureSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(device, ssboSetLayout, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, inFlightFences[i], nullptr);
    }

    for (size_t i = 0; i < renderFinishedSemaphores.size(); i++) {
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
    }

    vkDestroyCommandPool(device, commandPool, nullptr);

    vkDestroyDevice(device, nullptr);

#ifdef ENABLE_VULKAN_VALIDATION_LAYERS
    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
#endif

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();
}

void HammerEngine::addMeshRenderer(HammerMesh* mesh) {
    meshes.push_back(mesh);
}

void HammerEngine::removeMeshRenderer(int index){
    if (index < 0 || static_cast<size_t>(index) >= meshes.size()) {
        std::cerr << "Warning, trying to remove mesh from renderer with index out of range !!!\n";
        return;
    }
    std::swap(meshes[index], meshes.back());
    HammerMesh* deleteMeshPtr = meshes.back();
    meshes.pop_back();
    delete deleteMeshPtr;
}
