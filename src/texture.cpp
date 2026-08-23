/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the MIT license
 * LICENSE file in the root directory of this source tree.
 */


#include "../include/HammerEngine/HammerEngine.h"
#include "../lib/stb_truetype.h"
#include "../lib/stb_image.h"


HammerCustomTexture::HammerCustomTexture(HammerEngine* eng, unsigned char* bitmapData, uint32_t width, uint32_t height, HammerTextureFilter filter) 
    : engine(eng) {
    
    createTextureImage(bitmapData, width, height);
    imageView = engine->createImageView(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
    createTextureSampler(filter);
    
    allocateDescriptorSet(); 
    
    if (descriptorSet == VK_NULL_HANDLE) {
        throw std::runtime_error("HammerCustomTexture Error: Descriptor set is null!");
    }
}

HammerCustomTexture::~HammerCustomTexture() {
    vkDestroySampler(engine->getDevice(), sampler, nullptr);
    vkDestroyImageView(engine->getDevice(), imageView, nullptr);
    vkDestroyImage(engine->getDevice(), image, nullptr);
    vkFreeMemory(engine->getDevice(), imageMemory, nullptr);
}

void HammerCustomTexture::createTextureImage(unsigned char* pixels, uint32_t texWidth, uint32_t texHeight) {
    VkDeviceSize imageSize = texWidth * texHeight * 4;
    unsigned char* rgbaPixels = (unsigned char*)malloc(imageSize);
    
    for (uint32_t i = 0; i < texWidth * texHeight; ++i) {
        rgbaPixels[i * 4 + 0] = pixels[i];       // R (White)
        rgbaPixels[i * 4 + 1] = pixels[i];       // G (White)
        rgbaPixels[i * 4 + 2] = pixels[i];       // B (White)
        rgbaPixels[i * 4 + 3] = pixels[i]; // A (Use the font bitmap for transparency)
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    engine->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                        stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(engine->getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, rgbaPixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(engine->getDevice(), stagingBufferMemory);

    free(rgbaPixels);

    engine->createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, 
                       VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory);

    engine->transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, 
                                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    engine->copyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(texWidth), 
                             static_cast<uint32_t>(texHeight));
    engine->transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkQueueWaitIdle(engine->getGraphicsQueue());

    vkDestroyBuffer(engine->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(engine->getDevice(), stagingBufferMemory, nullptr);
}

void HammerCustomTexture::allocateDescriptorSet() {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = engine->getDescriptorPool();
    allocInfo.descriptorSetCount = 1;
    VkDescriptorSetLayout temp = engine->getTextureSetLayout();
    allocInfo.pSetLayouts = &temp;

    if (vkAllocateDescriptorSets(engine->getDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate texture descriptor set!");
    }

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = imageView;
    imageInfo.sampler = sampler;

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(engine->getDevice(), 1, &descriptorWrite, 0, nullptr);
}

void HammerCustomTexture::createTextureSampler(HammerTextureFilter filter) {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    
    if (filter == HammerTextureFilter::Linear) {
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
    } else {
        samplerInfo.magFilter = VK_FILTER_NEAREST;
        samplerInfo.minFilter = VK_FILTER_NEAREST;
    }

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (vkCreateSampler(engine->getDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

HammerMesh* HammerEngine::CreateTextQuad(HammerEngine* engine, HammerPipeline* pipeline, HammerCustomTexture* textTexture, float x, float y, float width, float height) {
    std::vector<Vertex> vertices = {
        {{x,         y,          0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        {{x + width, y,          0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        {{x + width, y + height, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{x,         y + height, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    };

    std::vector<uint32_t> indices = {
        0, 1, 2, 
        2, 3, 0
    };
    return new HammerMesh(engine, pipeline, reinterpret_cast<HammerTexture*>(textTexture), vertices, indices);
}


void HammerTexture::createTextureImage(const std::string& path) {
    int texWidth, texHeight, texChannels;
    // Load image using stb_image
    stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image: " + path);
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    engine->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                        stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(engine->getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(engine->getDevice(), stagingBufferMemory);

    stbi_image_free(pixels);

    engine->createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, 
                       VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory);

    engine->transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, 
                                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    engine->copyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(texWidth), 
                             static_cast<uint32_t>(texHeight));
    engine->transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(engine->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(engine->getDevice(), stagingBufferMemory, nullptr);
}

void HammerTexture::allocateDescriptorSet() {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = engine->getDescriptorPool();
    allocInfo.descriptorSetCount = 1;
    VkDescriptorSetLayout temp = engine->getTextureSetLayout();
    allocInfo.pSetLayouts = &temp;

    if (vkAllocateDescriptorSets(engine->getDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate texture descriptor set!");
    }

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = imageView;
    imageInfo.sampler = sampler;

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(engine->getDevice(), 1, &descriptorWrite, 0, nullptr);
}

void HammerTexture::createTextureSampler(HammerTextureFilter filter) {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    
    if (filter == HammerTextureFilter::Linear) {
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
    } else {
        samplerInfo.magFilter = VK_FILTER_NEAREST;
        samplerInfo.minFilter = VK_FILTER_NEAREST;
    }

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    
    // Anisotropy (Optional, but looks much better) idk what i will do with this

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(engine->getPhysicalDevice(), &properties);

    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (vkCreateSampler(engine->getDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

HammerTexture::HammerTexture(HammerEngine* eng, const std::string& path, HammerTextureFilter filter) 
    : engine(eng) {
    
    createTextureImage(path);
    imageView = engine->createImageView(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
    createTextureSampler(filter);
    
    allocateDescriptorSet(); 
    
    if (descriptorSet == VK_NULL_HANDLE) {
        throw std::runtime_error("HammerTexture Error: Descriptor set is null for " + path);
    }
}

HammerTexture::~HammerTexture() {
    vkDestroySampler(engine->getDevice(), sampler, nullptr);
    vkDestroyImageView(engine->getDevice(), imageView, nullptr);
    vkDestroyImage(engine->getDevice(), image, nullptr);
    vkFreeMemory(engine->getDevice(), imageMemory, nullptr);
    
    // Note: Descriptor Sets are usually freed automatically when the pool is destroyed i think, well i fucking hope
}

HammerTexture* HammerMesh::GetTexture() {
    return this->texture;
}
