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

class VkPixelCmdPool {
public:
    using Ptr = std::shared_ptr<VkPixelCmdPool>;
    static Ptr create(const VkPixelDevice::Ptr& device, VkCommandPoolCreateFlagBits flag = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT) {
        return std::make_shared<VkPixelCmdPool>(device, flag);
    }

    VkPixelCmdPool(const VkPixelDevice::Ptr &device, VkCommandPoolCreateFlagBits flag = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    ~VkPixelCmdPool();

    [[nodiscard]] auto getCommandPool() const { return mCommandPool; }

private:
    VkCommandPool mCommandPool{ VK_NULL_HANDLE };
    VkPixelDevice::Ptr mDevice{ nullptr };
};
}
#endif /* VkPixelCmdPool_hpp */
