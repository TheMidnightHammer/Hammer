/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the GPL 3.0 license
 * LICENSE file in the root directory of this source tree.
 */

#ifndef HAMMERENGINE_H
#define HAMMERENGINE_H

#include <chrono>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include <array>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <glm/gtx/hash.hpp>
#include <cstdint>
#include <cstring>
#include <vulkan/vulkan_core.h>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "../../lib/tiny_obj_loader.h"

class AsyncLogger {
private:
    std::queue<std::string> logQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::thread workerThread;
    std::atomic<bool> running{true};

    void ProcessLogs();

public:
    AsyncLogger();
    ~AsyncLogger();

    void Log(const std::string& message);
};

// use it like this
// std::string frameData = "X: " + std::to_string(posX) + 
//                         " | Y: " + std::to_string(posY) + 
//                         " | Z: " + std::to_string(posZ);

// g_Logger.Log(frameData); 



struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;

    // Use 0.0 - 1.0 range
    // Ambient: White at 20% intensity
    alignas(16) glm::vec4 ambientLightColor{1.0f, 1.0f, 1.0f, 0.2f};

    // Light Position
    alignas(16) glm::vec3 lightPosition{0.0f, 0.0f, 0.0f}; 
    
    // PADDING: This ensures lightColor starts on a 16-byte boundary
    float padding; 

    // Light Color: Pure white at full intensity
    alignas(16) glm::vec4 lightColor{1.0f, 1.0f, 1.0f, 1.0f};
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normal;

    bool operator==(const Vertex& other) const {
        return pos == other.pos && 
               color == other.color && 
               texCoord == other.texCoord &&
               normal == other.normal; // Include normal in equality check
    }

    static VkVertexInputBindingDescription getBindingDescription();
    // Change array size from 3 to 4
    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions(); 
};

class HammerEngine;
class HammerPipeline;
class HammerSSBO;

struct MeshPushConstants {
    glm::mat4 modelMatrix;
}; // prob wont change, to pass data to the shader just use SSBOs

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete();
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

//i dont really undersand this shit
namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                   (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.texCoord) << 1) ^
                   (hash<glm::vec3>()(vertex.normal) << 1);
        }
    };
}

class HammerModel {
public:
    std::vector<Vertex> vertexData;
    std::vector<uint32_t> indexData;

    HammerModel(const std::string& path);
};


enum class HammerTextureFilter {
    Nearest,
    Linear
};

class HammerTexture {
public:
    HammerTexture(HammerEngine& engine, const std::string& path, HammerTextureFilter filter);
    ~HammerTexture();

    HammerTexture(const HammerTexture&) = delete;
    HammerTexture& operator=(const HammerTexture&) = delete;

    VkDescriptorSet descriptorSet = VK_NULL_HANDLE; 

private:
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory imageMemory = VK_NULL_HANDLE;
    VkImageView imageView = VK_NULL_HANDLE;
    VkSampler sampler = VK_NULL_HANDLE;

    HammerEngine& engine;

    void createTextureImage(const std::string& path);
    void createTextureImageView();
    void createTextureSampler(HammerTextureFilter filter);
    void allocateDescriptorSet(); 
};

class HammerMesh {
public:
    HammerMesh(HammerEngine& engine, 
                HammerPipeline* pipeline, 
                HammerTexture* texture, 
                const std::vector<Vertex>& vertices, 
                const std::vector<uint32_t>& indices);

    
    ~HammerMesh();

    bool draw = true;

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale    = glm::vec3(1.0f);

    std::vector<Vertex> vertexData;
    std::vector<uint32_t> indexData;

    HammerMesh(const HammerMesh&) = delete;
    HammerMesh& operator=(const HammerMesh&) = delete;

    void bindAndDraw(VkCommandBuffer commandBuffer, uint32_t currentFrame);
    
    HammerPipeline* getPipeline() const { return pipeline; }
    HammerTexture* getTexture() const { return texture; }
    
    void updateBuffers(std::vector<Vertex> vertexData, std::vector<uint32_t> indexData);

    HammerPipeline* GetPipeline();
    HammerTexture* GetTexture();

    uint32_t GetIndexCount();

private:

    void createVertexBuffer(const std::vector<Vertex>& vertices);
    void createIndexBuffer(const std::vector<uint32_t>& indices);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    HammerEngine& engine;

    HammerPipeline* pipeline;
    HammerTexture* texture;

    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer indexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

    uint32_t indexCount;
};

class HammerPipeline {
public:
    HammerPipeline(
        HammerEngine& engine, 
        std::string& vertPath, 
        std::string& fragPath,
        int renderTriangleMod,
        bool triangleRender2SideMode,
        HammerSSBO* targetSSBO
    );

        // TODO:
    // HammerMesh(HammerSSBO* targetSSBO) : ssbo(targetSSBO) {
    //     // i can now access ssbo->getBuffer() when setting up your 
    //     // vkUpdateDescriptorSets or binding resources.
    // }

    ~HammerPipeline();

    HammerPipeline(const HammerPipeline&) = delete;
    HammerPipeline& operator=(const HammerPipeline&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    void createGraphicsPipeline(
        std::string& vertPath, 
        std::string& fragPath,
        int renderTriangleMod,
        bool triangleRender2SideMode);

    HammerEngine& hammerEngine;
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;    
    HammerSSBO* ssbo = nullptr;
};


class HammerEngine {
public:

    uint32_t WindowWidth;
    uint32_t WindowHeight;

    bool enableValidationLayers;

    void addMeshRenderer(HammerMesh* mesh);

    float yaw = -90.0f; // Horizontal angle
    float pitch = 0.0f; // Vertical angle
    float cameraSpeed = 0.01f; // Speed of movement
    float sensitivity = 0.05f; 
    bool mouse_capture = true;

    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f); // Initial position
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);   // Forward direction
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);       // Up direction

    std::vector<HammerMesh*> meshs;

    int MaxTextures = 1000;

    GLFWwindow* window; // GLFW window

    std::string windowName = "Hammer Engine";

    void runTest();

    int mouseLock = 1;

    float renderDistance = 512.0f; // how fare the camera can see, bigger the numbre bigger the buffers more memory needed

    float cameraFallSpeed = 0.01; // how fast the camera falls in gravaty enabled.

    void setMaxVertciesIndicesSize(VkDeviceSize maxsize); // giving the maximume size that the vertcies and incices can go to.

    void cleanup(); // engine clean up.

    void drawPassStart();

    void drawPassEnd();

    void initWindow(); // GLFW window init

    void initVulkan(); // vulkan init

    void updateCameraDefaultGravety3D(bool fall);

    void updateCameraDefault3D();

    void updateCameraDefault2D();

    void updateFrameTimeEnd();

    void drawFrame(); // function to call a new frame

    void updateFrameTimeStart();

    void removeMeshRenderer(int index);

    double currentTime = glfwGetTime();
    std::chrono::time_point<std::chrono::high_resolution_clock> start;

	double mouseX;
	double mouseY;

	float lastX = 0, lastY = 0;
	bool firstMouse = true;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkRenderPass renderPass;
	VkDescriptorSetLayout globalSetLayout; 
    VkDescriptorSetLayout textureSetLayout;
    VkDescriptorSetLayout ssboSetLayout;
    std::vector<VkDescriptorSet> globalDescriptorSets;

	VkCommandPool commandPool;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
    VkDeviceSize stagingBufferSize = 0;

    VkBuffer stagingBuffer2 = VK_NULL_HANDLE;
    VkDeviceMemory stagingBuffer2Memory = VK_NULL_HANDLE;
    VkDeviceSize stagingBuffer2Size = 0;

    std::vector<VkFence> imagesInFlight;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	uint32_t currentFrame = 0;



    double previousTime = glfwGetTime();
    int frameCount = 0;

    std::chrono::milliseconds totalFrameTime;

	bool framebufferResized = false;

    void createStagingBuffer();

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    void mouseCallback(double xpos, double ypos);

    void mainLoopTest();

    void cleanupSwapChain();

    void recreateSwapChain();

    void createInstance();

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    void setupDebugMessenger();

    void createSurface();

    void pickPhysicalDevice();

    void createLogicalDevice();

    void createSwapChain();

    void createImageViews();

    void createRenderPass();

    void createDescriptorSetLayout();

    void createGraphicsPipeline();

    void createFramebuffers();

    void createCommandPool();

    void createDepthResources();

    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    VkFormat findDepthFormat();

    bool hasStencilComponent(VkFormat format);

    void createTextureImage();

    void createTextureImageView();

    void createTextureSampler();

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    void createVertexBuffer();

    void createIndexBuffer();

    void createUniformBuffers();

    void createDescriptorPool();

    void createDescriptorSets();

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    VkCommandBuffer beginSingleTimeCommands();

    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void createCommandBuffers();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void createSyncObjects();

    void updateUniformBuffer(uint32_t currentImage);

    VkShaderModule createShaderModule(const std::vector<char>& code);

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    bool isDeviceSuitable(VkPhysicalDevice device);

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    std::vector<const char*> getRequiredExtensions();

    bool checkValidationLayerSupport();

    std::vector<char> readFile(std::string& filename);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

};

class HammerSSBO {
private:
    HammerEngine* engine;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
    VkDeviceSize bufferSize;
    VkDescriptorSet SsboDescriptorSet = VK_NULL_HANDLE;

public:
    HammerSSBO(HammerEngine* engine, const void* data, VkDeviceSize size);

    ~HammerSSBO();

    VkBuffer getBuffer();
    VkDeviceSize getSize();
    VkDescriptorSet getDescriptorSet();

    void updateData(const void* newData, VkDeviceSize newSize);

private:
    void createStorageBuffer(const void* data);

    void allocateAndWriteDescriptorSet();
};

#endif
