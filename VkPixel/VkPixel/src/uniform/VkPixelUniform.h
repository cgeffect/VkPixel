//
//  VkPixelUniform.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelUniform_hpp
#define VkPixelUniform_hpp

#include <stdio.h>
#include "../common/VkPixelBase.h"
#include "../cmdbuf/VkPixelBuffer.h"
#include "VkPixelDescriptorSetLayout.h"
#include "VkPixelDescriptorPool.h"
#include "VkPixelDescriptorSet.h"
#include "VkPixelDescriptor.h"
#include "../device/VkPixelDevice.h"
#include "../cmdbuf/VkPixelCmdPool.h"

namespace vkpixel {

class VkPixelUniform {
public:
    using Ptr = std::shared_ptr<VkPixelUniform>;
    static Ptr create() { return std::make_shared<VkPixelUniform>(); }

    VkPixelUniform();

    ~VkPixelUniform();

    void init(const VkPixelDevice::Ptr &device, const VkPixelCmdPool::Ptr &commandPool, int frameCount);

    void update(const VPMatrices &vpMatrices, const ObjectUniform &objectUniform, const int& frameCount);

    [[nodiscard]] auto getDescriptorLayout() const { return mDescriptorSetLayout; }

    [[nodiscard]] auto getDescriptorSet(int frameCount) const { return mDescriptorSet->getDescriptorSet(frameCount); }

private:
    VkPixelDevice::Ptr mDevice{ nullptr };
    std::vector<UniformParameter::Ptr> mUniformParams;

    VkPixelDescriptorSetLayout::Ptr mDescriptorSetLayout{ nullptr };
    VkPixelDescriptorPool::Ptr    mDescriptorPool{ nullptr };
    VkPixelDescriptorSet::Ptr        mDescriptorSet{ nullptr };
};
}

#endif /* VkPixelUniform_hpp */
