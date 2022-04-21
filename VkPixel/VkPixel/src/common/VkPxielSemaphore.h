//
//  VkPxielSemaphore.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPxielSemaphore_hpp
#define VkPxielSemaphore_hpp

#include <stdio.h>

#include "../common/VkPixelBase.h"
#include "../device/VkPixelDevice.h"

namespace vkpixel {

class VkPxielSemaphore {
public:
    using Ptr = std::shared_ptr<VkPxielSemaphore>;
    static Ptr create(const VkPixelDevice::Ptr &device) { return std::make_shared<VkPxielSemaphore>(device); }

    VkPxielSemaphore(const VkPixelDevice::Ptr& device);

    ~VkPxielSemaphore();

    [[nodiscard]] auto getSemaphore() const { return mSemaphore; }
private:
    VkSemaphore mSemaphore{ VK_NULL_HANDLE };
    VkPixelDevice::Ptr mDevice{ nullptr };
};
}

#endif /* VkPxielSemaphore_hpp */
