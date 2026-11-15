/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the MIT license
 * LICENSE file in the root directory of this source tree.
 */

#include "EngineCommon.h"

#include <vulkan/vulkan_core.h>

uint32_t HammerMesh::getIndexCount() {
    return indexCount;
}

void HammerMesh::bindAndDraw(VkCommandBuffer commandBuffer, uint32_t currentFrame) {
    if(this->draw){
        if (pipeline == nullptr){
            std::cerr << "WARNING: pipeline is NULL\n";
            return;
        }
        if (texture == nullptr){
            std::cerr << "WARNING: texture is NULL\n";
            return;
        }

        if (currentFrame >= engine->getGlobalDescriptorSets().size() ||
            engine->getGlobalDescriptorSets()[currentFrame] == VK_NULL_HANDLE) {
            return;
        }


        VkBuffer vertexBuffers[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineLayout, 0, 1, &engine->getGlobalDescriptorSets()[currentFrame], 0, nullptr);

        if (texture != nullptr){ // this means there is no texture, mesh without texture
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineLayout, 1, 1, &texture->descriptorSet, 0, nullptr);
        }

        if (pipeline->SSBO != nullptr && pipeline->SSBO->getDescriptorSet() != VK_NULL_HANDLE) {
            VkDescriptorSet SSBODescriptorSet = pipeline->SSBO->getDescriptorSet(); // compiler was yapping too much
            vkCmdBindDescriptorSets(
                commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipeline->pipelineLayout,
                2,
                1,
                &SSBODescriptorSet,
                0,
                nullptr
            );
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        model = glm::scale(model, scale);

        // TODO: remove GLM, less dependencies less trouble, -TheMidnightHammer

        MeshPushConstants push{};
        push.modelMatrix = model;

        vkCmdPushConstants(
            commandBuffer,
            pipeline->pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT,
            0,
            sizeof(MeshPushConstants),
            &push
        );

        vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
    }
}


void HammerMesh::syncVertexIndexBuffers() {
    VkDeviceSize newVertexSize = this->vertexData.size(); //sizeof(Vertex) * newVertexData.size();
    VkDeviceSize newIndexSize = this->indexData.size(); //sizeof(uint32_t) * newIndexData.size();

    VkDeviceSize oldVertexSize = oldIndexData;
    VkDeviceSize oldIndexSize = oldIndexData;

    if(newVertexSize >= oldVertexSize || newIndexSize >= oldIndexSize) {
        std::cerr << "WARNING: Hammer must recreate recreate buffers to fit buffer data update, this may cause performance spikes !!!\n";


        // Scary !!!
        // but we need speed, a lot of speed
        //vkDeviceWaitIdle(engine->getDevice());

        if (vertexBuffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(engine->getDevice(), vertexBuffer, nullptr);
            vkFreeMemory(engine->getDevice(), vertexBufferMemory, nullptr);
        }
        if (indexBuffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(engine->getDevice(), indexBuffer, nullptr);
            vkFreeMemory(engine->getDevice(), indexBufferMemory, nullptr);
        }

        this->indexCount = static_cast<uint32_t>(indexData.size());

        this->createVertexBuffer(this->vertexData); // recreate vertex/index buffers but with updated vertices
        this->createIndexBuffer(this->indexData);
        return; // returning because recreating the buffers would have put new data in them
    }

    this->indexCount = static_cast<uint32_t>(indexData.size());

    if (newVertexSize == 0 || newIndexSize == 0) {
        std::cerr << "WARNING: vertex size and/or index size is 0\n";

        return;
    }

    void* vData;
    vkMapMemory(engine->getDevice(), engine->stagingBufferMemory, 0, newVertexSize, 0, &vData);
    memcpy(vData, vertexData.data(), (size_t)newVertexSize);
    vkUnmapMemory(engine->getDevice(), engine->stagingBufferMemory);

    void* iData;
    vkMapMemory(engine->getDevice(), engine->stagingBuffer2Memory, 0, newIndexSize, 0, &iData);
    memcpy(iData, indexData.data(), (size_t)newIndexSize);
    vkUnmapMemory(engine->getDevice(), engine->stagingBuffer2Memory);

    VkCommandBuffer commandBuffer = engine->beginSingleTimeCommands();

    VkBufferCopy vertexCopyRegion{};
    vertexCopyRegion.size = newVertexSize;
    vkCmdCopyBuffer(commandBuffer, engine->stagingBuffer, vertexBuffer, 1, &vertexCopyRegion);

    VkBufferCopy indexCopyRegion{};
    indexCopyRegion.size = newIndexSize;
    vkCmdCopyBuffer(commandBuffer, engine->stagingBuffer2, indexBuffer, 1, &indexCopyRegion);

    engine->endSingleTimeCommands(commandBuffer);

    // updated vertex/index size count
    oldIndexData = this->indexData.size();
    oldVertexData = this->vertexData.size();
}

HammerMesh::HammerMesh(Hammer* engine, HammerPipeline* pipeline, HammerTexture* texture, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
                       : engine(engine), pipeline(pipeline), texture(texture) {

    indexCount = static_cast<uint32_t>(indices.size());


    if (vertices.empty() || indices.size() == 0) {
        std::cerr << "HammerMesh Error: Cannot create mesh with 0 vertices/indices!" << std::endl;
        return;
    }

    this->vertexData = vertices;
    this->indexData = indices;
    this->indexCount = static_cast<uint32_t>(indices.size());

    createVertexBuffer(vertices);
    createIndexBuffer(indices);

    // updated vertex/index size count
    oldIndexData = this->indexData.size();
    oldVertexData = this->vertexData.size();
}

HammerMesh::~HammerMesh() {
    if (vertexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(engine->getDevice(), vertexBuffer, nullptr);
    }
    if (vertexBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(engine->getDevice(), vertexBufferMemory, nullptr);
    }
    if (indexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(engine->getDevice(), indexBuffer, nullptr);
    }
    if (indexBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(engine->getDevice(), indexBufferMemory, nullptr);
    }
}

void HammerMesh::createVertexBuffer(const std::vector<Vertex>& vertices) {


    /*
    OPTIMIZATION NOT:

    Create one staging buffer (vertex and index) and just reusing them insted of creating a new one
    */

    if (vertices.empty()) return;

    uint64_t totalElements = static_cast<uint64_t>(vertices.size());
    uint64_t elementSize = static_cast<uint64_t>(sizeof(Vertex));
    uint64_t bufferSizeIndex = totalElements * elementSize;

    //                                 byte       kb        mb       gb
    constexpr uint64_t maxBufferSize = 1024ULL * 1024ULL * 1024ULL * 2ULL;

    VkDeviceSize bufferSize = static_cast<VkDeviceSize>(bufferSizeIndex);

    std::string errorMessage = "Mesh buffer size (" +
                               std::to_string(bufferSizeIndex / (1024 * 1024)) +
                               " MB) exceeds the 2048 MB maximum limit, try to seperate your mesh in sub-chunks or remove some data of the mesh";

    if(bufferSizeIndex > maxBufferSize){
        throw std::runtime_error(errorMessage);
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    engine->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );

    void* data;
    vkMapMemory(engine->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(engine->getDevice(), stagingBufferMemory);

    engine->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        vertexBuffer,
        vertexBufferMemory
    );

    engine->copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkQueueWaitIdle(engine->getGraphicsQueue());

    vkDestroyBuffer(engine->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(engine->getDevice(), stagingBufferMemory, nullptr);

    // updated vertex/index size count, do both because :D
    oldIndexData = this->indexData.size();
    oldVertexData = this->vertexData.size();
}

void HammerMesh::createIndexBuffer(const std::vector<uint32_t>& indices) {


    /*
    OPTIMIZATION NOT:

    Create one staging buffer (vertex and index) and just reusing them insted of creating a new one
    */



    if (indices.empty()) return;

    uint64_t totalElements = static_cast<uint64_t>(indices.size());
    uint64_t elementSize = static_cast<uint64_t>(sizeof(uint32_t));
    uint64_t bufferSizeIndex = totalElements * elementSize;

    //                                 byte       kb        mb       gb
    constexpr uint64_t maxBufferSize = 1024ULL * 1024ULL * 1024ULL * 2ULL;

    VkDeviceSize bufferSize = static_cast<VkDeviceSize>(bufferSizeIndex);

    std::string errorMessage = "Mesh buffer size (" +
                               std::to_string(bufferSizeIndex / (1024 * 1024)) +
                               " MB) exceeds the 2048 MB maximum limit, try to seperate your mesh in sub-chunks or remove some data of the mesh";

    if(bufferSizeIndex > maxBufferSize){
        throw std::runtime_error(errorMessage);
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    engine->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );

    void* data;
    vkMapMemory(engine->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(engine->getDevice(), stagingBufferMemory);

    engine->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        indexBuffer,
        indexBufferMemory
    );

    engine->copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(engine->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(engine->getDevice(), stagingBufferMemory, nullptr);

    // updated vertex/index size count, do both because :D
    oldIndexData = this->indexData.size();
    oldVertexData = this->vertexData.size();
}

VkVertexInputBindingDescription Vertex::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 4> Vertex::getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

    // 0: Position
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    // 1: Color
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    // 2: TexCoord
    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

    // 3: Normal
    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(Vertex, normal);

    return attributeDescriptions;
}
