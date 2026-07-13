#include "../include/HammerEngine/HammerEngine.h" 
#include <stdexcept>


HammerSSBO::HammerSSBO(HammerEngine* engine, const void* data, VkDeviceSize size) 
    : engine(engine), bufferSize(size) {
    
    createStorageBuffer(data);
    allocateAndWriteDescriptorSet();
}

HammerSSBO::~HammerSSBO() {
    if (buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(engine->getDevice(), buffer, nullptr);
    }
    if (bufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(engine->getDevice(), bufferMemory, nullptr);
    }
}

VkBuffer HammerSSBO::getBuffer() { return buffer; }
VkDeviceSize HammerSSBO::getSize() { return bufferSize; }
VkDescriptorSet HammerSSBO::getDescriptorSet() {    return SsboDescriptorSet; }

void HammerSSBO::updateData(const void* newData, VkDeviceSize newSize) {
    if (newData == nullptr || newSize == 0) return;

    if (bufferSize < newSize) {
        if (buffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(engine->getDevice(), buffer, nullptr);
        }
        if (bufferMemory != VK_NULL_HANDLE) {
            vkFreeMemory(engine->getDevice(), bufferMemory, nullptr);
        }
        bufferSize = newSize;

        createStorageBuffer(newData);
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = bufferSize;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = SsboDescriptorSet;
        descriptorWrite.dstBinding = 0; 
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(engine->getDevice(), 1, &descriptorWrite, 0, nullptr);

    } else {
        
        void* mappedData;
        vkMapMemory(engine->getDevice(), bufferMemory, 0, newSize, 0, &mappedData);
        memcpy(mappedData, newData, static_cast<size_t>(newSize));
        vkUnmapMemory(engine->getDevice(), bufferMemory);
    }
}

void HammerSSBO::createStorageBuffer(const void* data) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(engine->getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create storage buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(engine->getDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = engine->findMemoryType(
        memRequirements.memoryTypeBits, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    if (vkAllocateMemory(engine->getDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate storage buffer memory!");
    }

    vkBindBufferMemory(engine->getDevice(), buffer, bufferMemory, 0);

    if (data != nullptr) {
        void* mappedData;
        vkMapMemory(engine->getDevice(), bufferMemory, 0, bufferSize, 0, &mappedData);
        memcpy(mappedData, data, static_cast<size_t>(bufferSize));
        vkUnmapMemory(engine->getDevice(), bufferMemory);
    } else {
        throw std::runtime_error("ERROR: trying to create a shader storage buffer object without any data, data is nullptr\n");
    }
}

void HammerSSBO::allocateAndWriteDescriptorSet() {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    // Accessing engine fields directly:
    allocInfo.descriptorPool = engine->getDescriptorPool(); 
    allocInfo.descriptorSetCount = 1;
    VkDescriptorSetLayout temp = engine->getSsboSetLayout();
    allocInfo.pSetLayouts = &temp;

    if (vkAllocateDescriptorSets(engine->getDevice(), &allocInfo, &SsboDescriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate SSBO descriptor set!");
    }

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = bufferSize;

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = SsboDescriptorSet;
    descriptorWrite.dstBinding = 0; 
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(engine->getDevice(), 1, &descriptorWrite, 0, nullptr);
}