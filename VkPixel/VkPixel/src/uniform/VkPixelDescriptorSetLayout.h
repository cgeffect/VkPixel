//
//  VkPixelDescriptorSetLayout.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelSescriptorSetLayout_hpp
#define VkPixelSescriptorSetLayout_hpp

#include <stdio.h>

#include "../common/VkPixelBase.h"
#include "../device/VkPixelDevice.h"
#include "VkPixelDescriptor.h"

namespace vkpixel {

class VkPixelDescriptorSetLayout {
public:
    using Ptr = std::shared_ptr<VkPixelDescriptorSetLayout>;
    static Ptr create(const VkPixelDevice::Ptr& device) { return std::make_shared<VkPixelDescriptorSetLayout>(device); }

    VkPixelDescriptorSetLayout(const VkPixelDevice::Ptr &device);

    ~VkPixelDescriptorSetLayout();

    void build(const std::vector<UniformParameter::Ptr>& params);

    [[nodiscard]] auto getLayout() const { return mLayout; }

private:
    VkDescriptorSetLayout mLayout{ VK_NULL_HANDLE };
    VkPixelDevice::Ptr mDevice{ nullptr };

    std::vector<UniformParameter::Ptr> mParams{};
};
}

#endif /* VkPixelSescriptorSetLayout_hpp */
