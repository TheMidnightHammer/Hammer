/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the MIT license
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
#include "../../lib/stb_truetype.h"

class HammerAsyncLogger {
private:
    std::queue<std::string> logQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::thread workerThread;
    std::atomic<bool> running{true};

    void ProcessLogs();

public:
    HammerAsyncLogger();
    ~HammerAsyncLogger();

    void Log(const std::string& message);
};

// use it like this
// std::string frameData = "X: " + std::to_string(posX) + 
//                         " | Y: " + std::to_string(posY) + 
//                         " | Z: " + std::to_string(posZ);

// g_Logger.Log(frameData); 


class HammerInfo {
public:

    bool enableValidationLayers;
    int windowWidth;
    int windowHeight;
    std::string windowName;
    int MaxTextures;
    bool mouseLock;
    float cameraSpeed;
    float renderDistance;
    VkDeviceSize stagingBufferSize;

    HammerInfo(bool EnableValidationLayers,
                int WindowWidth,
                int WindowHeight,
                std::string WindowName,
                int maxTextures,
                bool MouseLock,
                float CameraSpeed,
                float RenderDistance,
                VkDeviceSize StagingBufferSize) : enableValidationLayers(EnableValidationLayers),
                                                windowHeight(WindowHeight), windowWidth(WindowWidth),
                                                windowName(WindowName), MaxTextures(maxTextures),
                                                mouseLock(MouseLock), cameraSpeed(CameraSpeed),
                                                renderDistance(RenderDistance), stagingBufferSize(StagingBufferSize) {}
};

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
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

// forward declaration of classes

class HammerEngine;
class HammerPipeline;
class HammerSSBO;
class HammerFont;
class HammerTexture;
class HammerCostumTexture;

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

enum class HammerTextureFilter {
    Nearest,
    Linear
};

class HammerCustomTexture {
public:
    HammerCustomTexture(HammerEngine& Engine, unsigned char* bitmapData, uint32_t width, uint32_t height, HammerTextureFilter filter);
    ~HammerCustomTexture();

    HammerCustomTexture(const HammerCustomTexture&) = delete;
    HammerCustomTexture& operator=(const HammerCustomTexture&) = delete;

    VkDescriptorSet descriptorSet = VK_NULL_HANDLE; 

private:
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory imageMemory = VK_NULL_HANDLE;
    VkImageView imageView = VK_NULL_HANDLE;
    VkSampler sampler = VK_NULL_HANDLE;

    HammerEngine& engine;

    void createTextureImage(unsigned char* pixels, uint32_t texWidth, uint32_t texHeight);
    void createTextureSampler(HammerTextureFilter filter);
    void allocateDescriptorSet(); 
};

struct Char {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    char charName;
};

class HammerFont {
public:
    HammerFont(HammerEngine& engine,const char* fontPath);
    HammerCustomTexture* createTextPtr(HammerEngine& engine, const char* word, unsigned char index, const unsigned int l_h, const unsigned int b_w, const unsigned int b_h, HammerTextureFilter filter);
    HammerCustomTexture* createFontAtlasPtr(HammerEngine& engine, unsigned char index, const unsigned int l_h, const unsigned int b_w, const unsigned int b_h, HammerTextureFilter filter);

    Char charCoor(int index, int totalChars, float xOffset, float yOffset, float charWidth, float charHeight, const char name);

    void convertTextToVertexAndIndex(std::vector<Vertex>* verticesToAdd, std::vector<uint32_t>* indicesToAdd, std::string textToConvertToVertex);

    Char space = charCoor(0, 95, 0, 0, 0.8f, 3.0f, ' ');
    Char exclamation = charCoor(1, 95, 0, 0, 0.8f, 3.0f, '!');
    Char double_quote = charCoor(2, 95, 0, 0, 0.8f, 3.0f, '"');
    Char hash = charCoor(3, 95, 0, 0, 0.8f, 3.0f, '#');
    Char dollar = charCoor(4, 95, 0, 0, 0.8f, 3.0f, '$');
    Char percent = charCoor(5, 95, 0, 0, 0.8f, 3.0f, '%');
    Char ampersand = charCoor(6, 95, 0, 0, 0.8f, 3.0f, '&');
    Char single_quote = charCoor(7, 95, 0, 0, 0.8f, 3.0f, '\'');
    Char open_paren = charCoor(8, 95, 0, 0, 0.8f, 3.0f, '(');
    Char close_paren = charCoor(9, 95, 0, 0, 0.8f, 3.0f, ')');
    Char asterisk = charCoor(10, 95, 0, 0, 0.8f, 3.0f, '*');
    Char plus = charCoor(11, 95, 0, 0, 0.8f, 3.0f, '+');
    Char comma = charCoor(12, 95, 0, 0, 0.8f, 3.0f, ',');
    Char minus = charCoor(13, 95, 0, 0, 0.8f, 3.0f, '-');
    Char period = charCoor(14, 95, 0, 0, 0.8f, 3.0f, '.');
    Char slash = charCoor(15, 95, 0, 0, 0.8f, 3.0f, '/');
    Char num_0 = charCoor(16, 95, 0, 0, 0.8f, 3.0f, '0');
    Char num_1 = charCoor(17, 95, 0, 0, 0.8f, 3.0f, '1');
    Char num_2 = charCoor(18, 95, 0, 0, 0.8f, 3.0f, '2');
    Char num_3 = charCoor(19, 95, 0, 0, 0.8f, 3.0f, '3');
    Char num_4 = charCoor(20, 95, 0, 0, 0.8f, 3.0f, '4');
    Char num_5 = charCoor(21, 95, 0, 0, 0.8f, 3.0f, '5');
    Char num_6 = charCoor(22, 95, 0, 0, 0.8f, 3.0f, '6');
    Char num_7 = charCoor(23, 95, 0, 0, 0.8f, 3.0f, '7');
    Char num_8 = charCoor(24, 95, 0, 0, 0.8f, 3.0f, '8');
    Char num_9 = charCoor(25, 95, 0, 0, 0.8f, 3.0f, '9');
    Char colon = charCoor(26, 95, 0, 0, 0.8f, 3.0f, ':');
    Char semicolon = charCoor(27, 95, 0, 0, 0.8f, 3.0f, ';');
    Char less_than = charCoor(28, 95, 0, 0, 0.8f, 3.0f, '<');
    Char equal = charCoor(29, 95, 0, 0, 0.8f, 3.0f, '=');
    Char greater_than = charCoor(30, 95, 0, 0, 0.8f, 3.0f, '>');
    Char question_mark = charCoor(31, 95, 0, 0, 0.8f, 3.0f, '?');
    Char at = charCoor(32, 95, 0, 0, 0.8f, 3.0f, '@');
    Char A = charCoor(33, 95, 0, 0, 0.8f, 3.0f, 'A');
    Char B = charCoor(34, 95, 0, 0, 0.8f, 3.0f, 'B');
    Char C = charCoor(35, 95, 0, 0, 0.8f, 3.0f, 'C');
    Char D = charCoor(36, 95, 0, 0, 0.8f, 3.0f, 'D');
    Char E = charCoor(37, 95, 0, 0, 0.8f, 3.0f, 'E');
    Char F = charCoor(38, 95, 0, 0, 0.8f, 3.0f, 'F');
    Char G = charCoor(39, 95, 0, 0, 0.8f, 3.0f, 'G');
    Char H = charCoor(40, 95, 0, 0, 0.8f, 3.0f, 'H');
    Char I = charCoor(41, 95, 0, 0, 0.8f, 3.0f, 'I');
    Char J = charCoor(42, 95, 0, 0, 0.8f, 3.0f, 'J');
    Char K = charCoor(43, 95, 0, 0, 0.8f, 3.0f, 'K');
    Char L = charCoor(44, 95, 0, 0, 0.8f, 3.0f, 'L');
    Char M = charCoor(45, 95, 0, 0, 0.8f, 3.0f, 'M');
    Char N = charCoor(46, 95, 0, 0, 0.8f, 3.0f, 'N');
    Char O = charCoor(47, 95, 0, 0, 0.8f, 3.0f, 'O');
    Char P = charCoor(48, 95, 0, 0, 0.8f, 3.0f, 'P');
    Char Q = charCoor(49, 95, 0, 0, 0.8f, 3.0f, 'Q');
    Char R = charCoor(50, 95, 0, 0, 0.8f, 3.0f, 'R');
    Char S = charCoor(51, 95, 0, 0, 0.8f, 3.0f, 'S');
    Char T = charCoor(52, 95, 0, 0, 0.8f, 3.0f, 'T');
    Char U = charCoor(53, 95, 0, 0, 0.8f, 3.0f, 'U');
    Char V = charCoor(54, 95, 0, 0, 0.8f, 3.0f, 'V');
    Char W = charCoor(55, 95, 0, 0, 0.8f, 3.0f, 'W');
    Char X = charCoor(56, 95, 0, 0, 0.8f, 3.0f, 'X');
    Char Y = charCoor(57, 95, 0, 0, 0.8f, 3.0f, 'Y');
    Char Z = charCoor(58, 95, 0, 0, 0.8f, 3.0f, 'Z');
    Char open_bracket = charCoor(59, 95, 0, 0, 0.8f, 3.0f, '[');
    Char backslash = charCoor(60, 95, 0, 0, 0.8f, 3.0f, '\\');
    Char close_bracket = charCoor(61, 95, 0, 0, 0.8f, 3.0f, ']');
    Char caret = charCoor(62, 95, 0, 0, 0.8f, 3.0f, '^');
    Char underscore = charCoor(63, 95, 0, 0, 0.8f, 3.0f, '_');
    Char backtick = charCoor(64, 95, 0, 0, 0.8f, 3.0f, '`');
    Char a = charCoor(65, 95, 0, 0, 0.8f, 3.0f, 'a');
    Char b = charCoor(66, 95, 0, 0, 0.8f, 3.0f, 'b');
    Char c = charCoor(67, 95, 0, 0, 0.8f, 3.0f, 'c');
    Char d = charCoor(68, 95, 0, 0, 0.8f, 3.0f, 'd');
    Char e = charCoor(69, 95, 0, 0, 0.8f, 3.0f, 'e');
    Char f = charCoor(70, 95, 0, 0, 0.8f, 3.0f, 'f');
    Char g = charCoor(71, 95, 0, 0, 0.8f, 3.0f, 'g');
    Char h = charCoor(72, 95, 0, 0, 0.8f, 3.0f, 'h');
    Char i = charCoor(73, 95, 0, 0, 0.8f, 3.0f, 'i');
    Char j = charCoor(74, 95, 0, 0, 0.8f, 3.0f, 'j');
    Char k = charCoor(75, 95, 0, 0, 0.8f, 3.0f, 'k');
    Char l = charCoor(76, 95, 0, 0, 0.8f, 3.0f, 'l');
    Char m = charCoor(77, 95, 0, 0, 0.8f, 3.0f, 'm');
    Char n = charCoor(78, 95, 0, 0, 0.8f, 3.0f, 'n');
    Char o = charCoor(79, 95, 0, 0, 0.8f, 3.0f, 'o');
    Char p = charCoor(80, 95, 0, 0, 0.8f, 3.0f, 'p');
    Char q = charCoor(81, 95, 0, 0, 0.8f, 3.0f, 'q');
    Char r = charCoor(82, 95, 0, 0, 0.8f, 3.0f, 'r');
    Char s = charCoor(83, 95, 0, 0, 0.8f, 3.0f, 's');
    Char t = charCoor(84, 95, 0, 0, 0.8f, 3.0f, 't');
    Char u = charCoor(85, 95, 0, 0, 0.8f, 3.0f, 'u');
    Char v = charCoor(86, 95, 0, 0, 0.8f, 3.0f, 'v');
    Char w = charCoor(87, 95, 0, 0, 0.8f, 3.0f, 'w');
    Char x = charCoor(88, 95, 0, 0, 0.8f, 3.0f, 'x');
    Char y = charCoor(89, 95, 0, 0, 0.8f, 3.0f, 'y');
    Char z = charCoor(90, 95, 0, 0, 0.8f, 3.0f, 'z');
    Char open_brace = charCoor(91, 95, 0, 0, 0.8f, 3.0f, '{');
    Char pipe = charCoor(92, 95, 0, 0, 0.8f, 3.0f, '|');
    Char close_brace = charCoor(93, 95, 0, 0, 0.8f, 3.0f, '}');
    Char tilde = charCoor(94, 95, 0, 0, 0.8f, 3.0f, '~');

    std::vector<Char> allCharVectors = {
        space, exclamation, double_quote, hash, dollar, percent, ampersand, single_quote,
        open_paren, close_paren, asterisk, plus, comma, minus, period, slash,
        num_0, num_1, num_2, num_3, num_4, num_5, num_6, num_7, num_8, num_9,
        colon, semicolon, less_than, equal, greater_than, question_mark, at,
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        open_bracket, backslash, close_bracket, caret, underscore, backtick,
        a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
        open_brace, pipe, close_brace, tilde
    };

private:
    stbtt_fontinfo* fonts = NULL;
    unsigned short fontSize = 0;

    HammerEngine& engine;
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

    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

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

    HammerEngine(HammerInfo info) {
        this->enableValidationLayers = info.enableValidationLayers;
        this->windowWidth = info.windowWidth;
        this->windowHeight = info.windowHeight;
        this->windowName = info.windowName;
        this->MaxTextures = info.MaxTextures;
        this->mouseLock = info.mouseLock;
        this->cameraSpeed = info.cameraSpeed;
        this->renderDistance = info.renderDistance;
        this->setMaxBufferSize(info.stagingBufferSize); // 16 mb of staging size
    }

    uint32_t windowWidth;
    uint32_t windowHeight;

    bool enableValidationLayers;

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

    GLFWmonitor* targetMonitor = nullptr;

    std::string windowName = "Hammer Engine";

    bool mouseLock = false;

    float renderDistance = 512.0f; // how fare the camera can see, bigger the numbre bigger the buffers more memory needed

    float cameraFallSpeed = 0.01; // how fast the camera falls in gravaty enabled.

    bool fullscreen = false;

    int FPS = 0;

    bool useImGui = false;

    void InitImgui();

    void addMeshRenderer(HammerMesh* mesh);

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

    HammerMesh* CreateTextQuad(HammerEngine& engine, HammerPipeline* pipeline, HammerCustomTexture* textTexture, float x, float y, float width, float height);

    void setMaxBufferSize(VkDeviceSize size);




    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    VkShaderModule createShaderModule(const std::vector<char>& code);

    std::vector<char> readFile(std::string& filename);

    VkCommandBuffer beginSingleTimeCommands();

    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

    VkDescriptorPool getDescriptorPool() {
        return descriptorPool;
    }

    VkDevice getDevice() {
        return device;
    }

    VkPhysicalDevice getPhysicalDevice() {
        return physicalDevice;
    }

    VkQueue getGraphicsQueue() {
        return graphicsQueue;
    }

    std::vector<VkDescriptorSet> getGlobalDescriptorSets() {
        return globalDescriptorSets;
    }

    VkRenderPass getRenderPass() {
        return renderPass;
    }

    VkDescriptorSetLayout getGlobalSetLayout(){
        return globalSetLayout;
    }

    VkDescriptorSetLayout getTextureSetLayout() {
        return textureSetLayout;
    }

    VkDescriptorSetLayout getSsboSetLayout() {
        return ssboSetLayout;
    }


    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
    VkDeviceSize stagingBufferSize = 0;

    VkBuffer stagingBuffer2 = VK_NULL_HANDLE;
    VkDeviceMemory stagingBuffer2Memory = VK_NULL_HANDLE;
    VkDeviceSize stagingBuffer2Size = 0;

private:

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

    std::vector<VkFence> imagesInFlight;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

    VkDescriptorPool imguiPool;

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

    void createTextureSampler();

    void createVertexBuffer();

    void createIndexBuffer();

    void createUniformBuffers();

    void createDescriptorPool();

    void createDescriptorSets();

    void createCommandBuffers();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void createSyncObjects();

    void updateUniformBuffer(uint32_t currentImage);

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    bool isDeviceSuitable(VkPhysicalDevice device);

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    std::vector<const char*> getRequiredExtensions();

    bool checkValidationLayerSupport();

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
