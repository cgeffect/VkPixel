//
//  VkPixelCommonPool.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelCommonPool_hpp
#define VkPixelCommonPool_hpp

#include <stdio.h>

#include "../common/VkPixelBase.h"
#include "../device/VkPixelDevice.h"

namespace vkpixel {

class VkPixelCommonPool {
public:
    using Ptr = std::shared_ptr<VkPixelCommonPool>;
    static Ptr create(const VkPixelDevice::Ptr& device, VkCommandPoolCreateFlagBits flag = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT) {
        return std::make_shared<VkPixelCommonPool>(device, flag);
    }

    VkPixelCommonPool(const VkPixelDevice::Ptr &device, VkCommandPoolCreateFlagBits flag = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    ~VkPixelCommonPool();

    [[nodiscard]] auto getCommandPool() const { return mCommandPool; }

private:
    VkCommandPool mCommandPool{ VK_NULL_HANDLE };
    VkPixelDevice::Ptr mDevice{ nullptr };
};
}
#endif /* VkPixelCommonPool_hpp */
