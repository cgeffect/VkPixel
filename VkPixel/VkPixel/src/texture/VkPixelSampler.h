//
//  VkPixelSampler.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelSampler_hpp
#define VkPixelSampler_hpp

#include <stdio.h>
#include "../common/VkPixelBase.h"
#include "../device/VkPixelDevice.h"

namespace vkpixel {

class VkPixelSampler {
public:
    using Ptr = std::shared_ptr<VkPixelSampler>;
    static Ptr create(const VkPixelDevice::Ptr& device) { return std::make_shared<VkPixelSampler>(device); }

    VkPixelSampler(const VkPixelDevice::Ptr &device);

    ~VkPixelSampler();

    [[nodiscard]] auto getSampler() const { return mSampler; }

private:
    VkPixelDevice::Ptr mDevice{ nullptr };
    VkSampler mSampler{ VK_NULL_HANDLE };
};
}

#endif /* VkPixelSampler_hpp */
