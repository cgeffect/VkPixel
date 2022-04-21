//
//  VkPixelCommandBuffer.cpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#include "VkPixelBuffer.h"
#include "../cmdbuf/VkPixelCmdBuffer.h"
#include"../cmdbuf/VkPixelCommonPool.h"

namespace vkpixel {
VkPixelBuffer::Ptr VkPixelBuffer::createVertexBuffer(const VkPixelDevice::Ptr& device, VkDeviceSize size, void* pData) {
    auto buffer = VkPixelBuffer::create(
        device, size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    buffer->updateBufferByStage(pData, size);

    return buffer;
}

VkPixelBuffer::Ptr VkPixelBuffer::createIndexBuffer(const VkPixelDevice::Ptr& device, VkDeviceSize size, void* pData) {
    auto buffer = VkPixelBuffer::create(
        device, size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    buffer->updateBufferByStage(pData, size);

    return buffer;
}
VkPixelBuffer::Ptr VkPixelBuffer::createUniformBuffer(const VkPixelDevice::Ptr& device, VkDeviceSize size, void* pData) {
    auto buffer = VkPixelBuffer::create(
        device, size,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    if (pData != nullptr) {
        buffer->updateBufferByStage(pData, size);
    }

    return buffer;
}

VkPixelBuffer::Ptr VkPixelBuffer::createStageBuffer(const VkPixelDevice::Ptr& device, VkDeviceSize size, void* pData) {
    auto buffer = VkPixelBuffer::create(
        device, size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    if (pData != nullptr) {
        buffer->updateBufferByMap(pData, size);
    }

    return buffer;
}


VkPixelBuffer::VkPixelBuffer(const VkPixelDevice::Ptr& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    mDevice = device;

    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(mDevice->getDevice(), &createInfo, nullptr, &mBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Error:failed to create buffer");
    }

    //创建显存空间
    VkMemoryRequirements memReq{};
    vkGetBufferMemoryRequirements(mDevice->getDevice(), mBuffer, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;

    //符合我上述buffer需求的内存类型的ID们！0x001 0x010
    allocInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits, properties);

    if (vkAllocateMemory(mDevice->getDevice(), &allocInfo, nullptr, &mBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Error: failed to allocate memory");
    }

    vkBindBufferMemory(mDevice->getDevice(), mBuffer, mBufferMemory, 0);

    mBufferInfo.buffer = mBuffer;
    mBufferInfo.offset = 0;
    mBufferInfo.range = size;
}

VkPixelBuffer::~VkPixelBuffer() {
    if (mBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(mDevice->getDevice(), mBuffer, nullptr);
    }

    if (mBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(mDevice->getDevice(), mBufferMemory, nullptr);
    }
}

uint32_t VkPixelBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(mDevice->getPhysicalDevice(), &memProps);

    //0x001 | 0x100 = 0x101  i = 0 第i个对应类型就是  1 << i 1   i = 1 0x010
    for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
        if ((typeFilter & (1 << i)) && ((memProps.memoryTypes[i].propertyFlags & properties) == properties)) {
            return i;
        }
    }

    throw std::runtime_error("Error: cannot find the property memory type!");
}

void VkPixelBuffer::updateBufferByMap(void* data, size_t size) {
    void* memPtr = nullptr;

    vkMapMemory(mDevice->getDevice(), mBufferMemory, 0, size, 0, &memPtr);
    memcpy(memPtr, data, size);
    vkUnmapMemory(mDevice->getDevice(), mBufferMemory);
}

void VkPixelBuffer::updateBufferByStage(void* data, size_t size) {
    auto stageBuffer = VkPixelBuffer::create(mDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    stageBuffer->updateBufferByMap(data, size);

    copyBuffer(stageBuffer->getBuffer(), mBuffer, static_cast<VkDeviceSize>(size));
}

void VkPixelBuffer::copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, VkDeviceSize size) {
    auto commandPool = VkPixelCommonPool::create(mDevice);
    auto commandBuffer = VkPixelCmdBuffer::create(mDevice, commandPool);

    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkBufferCopy copyInfo{};
    copyInfo.size = size;
    
    commandBuffer->copyBufferToBuffer(srcBuffer, dstBuffer, 1, { copyInfo });

    commandBuffer->end();

    commandBuffer->submitSync(mDevice->getGraphicQueue(), VK_NULL_HANDLE);
}
}
