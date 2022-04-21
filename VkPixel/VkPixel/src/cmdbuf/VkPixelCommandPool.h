//
//  VkPixelCmdPool.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelCmdPool_hpp
#define VkPixelCmdPool_hpp

#include <stdio.h>

#include "../common/VkPixelBase.h"
#include "../device/VkPixelDevice.h"

namespace vkpixel {

class VkPixelCommandPool {
public:
    using Ptr = std::shared_ptr<VkPixelCommandPool>;
    static Ptr create(const VkPixelDevice::Ptr& device, VkCommandPoolCreateFlagBits flag = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT) {
        return std::make_shared<VkPixelCommandPool>(device, flag);
    }

    VkPixelCommandPool(const VkPixelDevice::Ptr &device, VkCommandPoolCreateFlagBits flag = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    ~VkPixelCommandPool();

    [[nodiscard]] auto getCommandPool() const { return mCommandPool; }

private:
    VkCommandPool mCommandPool{ VK_NULL_HANDLE };
    VkPixelDevice::Ptr mDevice{ nullptr };
};
}
#endif /* VkPixelCmdPool_hpp */
