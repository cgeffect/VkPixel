//
//  VkPixelCommandBuffer.cpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#include "VkPixelBuffer.h"
#include "../cmdbuf/VkPixelCommandBuffer.h"
#include"../cmdbuf/VkPixelCommandPool.h"

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
    /*
     创建一个只GPU可见的顶点缓冲，用途为VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT，属性为VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT。
     */
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
    /*
     创建一个暂存缓冲，用途为VK_BUFFER_USAGE_TRANSFER_SRC_BIT，其属性由于也要充当CPU与GPU沟通的桥梁，所以与之前创建的顶点缓冲一样，为VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT。
     */
    auto buffer = VkPixelBuffer::create(
        device, size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT//CPU与GPU沟通的桥梁
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

    //缓冲创建好了，但是要注意，这里创建的“缓冲”对象只是一个标记（就像OpenGL一样），在 Vulkan 中我们还需要手动分配显存并绑定到这个缓冲标记上（OpenGL：我们不一样，我已经在幕后给你搞定了）。
    if (vkCreateBuffer(mDevice->getDevice(), &createInfo, nullptr, &mBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Error:failed to create buffer");
    }

    //创建显存空间
    VkMemoryRequirements memReq{};
    //获取缓冲所需内存类型
    vkGetBufferMemoryRequirements(mDevice->getDevice(), mBuffer, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    //具体分配大小。传入显存需求实例的vkGetBufferMemoryRequirements::size成员变量
    allocInfo.allocationSize = memReq.size;

    //符合我上述buffer需求的内存类型的ID们！0x001 0x010
    /*
     我们还需要保证客户端有能力去映射这一片显存中分配的 buffer 并且进行修改，这些属性（存在VkMemoryPropertyFlags枚举中）包括了以下两个掩码（这俩我们都需要）：
     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT：对 CPU 可见。
     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ：memcpy 之后不需要显式调用 flush 和 invalidate 系列指令。
     */
    allocInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits, properties);

    //分配内存并绑定到缓冲对象
    if (vkAllocateMemory(mDevice->getDevice(), &allocInfo, nullptr, &mBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Error: failed to allocate memory");
    }

    vkBindBufferMemory(mDevice->getDevice(), mBuffer, mBufferMemory, 0);

    //具体的缓冲对象
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

//不同位置的显存拥有不同的分配方式和性能，所以接下来我们要为想要分配的顶点缓冲的内存需求找到最合适的内存种类（Memory Type）。
uint32_t VkPixelBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    //memoryTypes：内存种类
    //memoryHeaps：分配来源
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
    auto commandPool = VkPixelCommandPool::create(mDevice);
    auto commandBuffer = VkPixelCommandBuffer::create(mDevice, commandPool);

    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkBufferCopy copyInfo{};
    copyInfo.size = size;
    
    commandBuffer->copyBufferToBuffer(srcBuffer, dstBuffer, 1, { copyInfo });

    commandBuffer->end();

    commandBuffer->submitSync(mDevice->getGraphicQueue(), VK_NULL_HANDLE);
}
}
