//
//  VkPixelDescriptorPool.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelDescriptorPool_hpp
#define VkPixelDescriptorPool_hpp

#include <stdio.h>

#include "../common/VkPixelBase.h"
#include "../device/VkPixelDevice.h"
#include "VkPixelDescriptor.h"

namespace vkpixel {

class VkPixelDescriptorPool {
public:
    using Ptr = std::shared_ptr<VkPixelDescriptorPool>;
    static Ptr create(const VkPixelDevice::Ptr& device) { return std::make_shared<VkPixelDescriptorPool>(device); }

    VkPixelDescriptorPool(const VkPixelDevice::Ptr &device);

    ~VkPixelDescriptorPool();

    void build(std::vector<UniformParameter::Ptr>& params, const int &frameCount);

    [[nodiscard]] auto getPool() const { return mPool; }

private:
    //需要知道，每一种uniform都有多少个，才能为其预留分配空间，所谓空间并不是uniformbuffer的大小
    //而是，根据每种uniform不同，那么描述符就不同，所以说空间指的是描述符的大小，蕴含在系统内部的
    VkDescriptorPool mPool{ VK_NULL_HANDLE };
    VkPixelDevice::Ptr mDevice{ nullptr };
};
}

#endif /* VkPixelDescriptorPool_hpp */
