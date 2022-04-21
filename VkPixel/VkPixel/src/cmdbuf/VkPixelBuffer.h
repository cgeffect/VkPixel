//
//  VkPixelBuffer.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelCommandBuffer_hpp
#define VkPixelCommandBuffer_hpp

#include <stdio.h>

#include "../common/VkPixelBase.h"
#include "../device/VkPixelDevice.h"

namespace vkpixel {

class VkPixelBuffer {

public:
    using Ptr = std::shared_ptr<VkPixelBuffer>;
    static Ptr create(const VkPixelDevice::Ptr& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
        return std::make_shared<VkPixelBuffer>(device, size, usage, properties);
    }

public:
    static Ptr createVertexBuffer(const VkPixelDevice::Ptr& device, VkDeviceSize size, void * pData);

    static Ptr createIndexBuffer(const VkPixelDevice::Ptr& device, VkDeviceSize size, void* pData);

    static Ptr createUniformBuffer(const VkPixelDevice::Ptr& device, VkDeviceSize size, void* pData = nullptr);

    static Ptr createStageBuffer(const VkPixelDevice::Ptr& device, VkDeviceSize size, void* pData = nullptr);

public:
    VkPixelBuffer(const VkPixelDevice::Ptr &device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

    ~VkPixelBuffer();

    /*
    * 1 通过内存Mapping的形式，直接对内存进行更改，适用于HostVisible类型的内存
    * 2 如果本内存是LocalOptimal， 那么就必须创建中间的StageBuffer，先复制到StageBuffer，再拷贝入目标Buffer
    */
    void updateBufferByMap(void *data, size_t size);

    void updateBufferByStage(void* data, size_t size);

    void copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, VkDeviceSize size);

    [[nodiscard]] auto getBuffer() const { return mBuffer; }

    [[nodiscard]] VkDescriptorBufferInfo& getBufferInfo() { return mBufferInfo; }
    
private:
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
    VkBuffer mBuffer{ VK_NULL_HANDLE };
    VkDeviceMemory mBufferMemory{ VK_NULL_HANDLE };
    VkPixelDevice::Ptr mDevice{ nullptr };
    VkDescriptorBufferInfo mBufferInfo{};
};
}

#endif /* VkPixelCommandBuffer_hpp */
