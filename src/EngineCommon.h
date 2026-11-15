#ifndef HAMMER_COMMON_H
#define HAMMER_COMMON_H

#define GLFW_INCLUDE_VULKAN
#include "../include/Hammer/Hammer.h"

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <array>
#include <set>

inline uint32_t windowWidth = 800;
inline uint32_t windowHeight = 600;

inline constexpr int MAX_FRAMES_IN_FLIGHT = 2;

/*
 * Vulkan validation layers are compiled in only when ENABLE_VULKAN_VALIDATION_LAYERS
 * is defined (the CMake option adds it to the C++ flags of every source file):
 *   cmake -DENABLE_VULKAN_VALIDATION_LAYERS=ON ..
 * When not defined, all validation layer code is compiled out.
 */

inline const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef ENABLE_VULKAN_VALIDATION_LAYERS
inline const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};
#endif

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

#endif
