//
//  VkPixelDescriptorSet.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelDescriptorSet_hpp
#define VkPixelDescriptorSet_hpp

#include <stdio.h>
#include "../common/VkPixelBase.h"
#include "../device/VkPixelDevice.h"
#include "VkPixelDescriptor.h"
#include "VkPixelDescriptorSetLayout.h"
#include "VkPixelDescriptorPool.h"

namespace vkpixel {
/*
* 对于每一个模型的渲染，都需要绑定一个DescriptorSet，绑定的位置就是在CommandBuffer
* 一个DescriptorSet里面，都对应着一个vp矩阵使用的buffer，一个model矩阵使用的buffer，等等,其中也包括
* binding size等等的描述信息
* 由于交换链的存在，多帧有可能并行渲染，所以我们需要为每一个交换链的图片，对应生成一个DescriptorSet
*/

class VkPixelDescriptorSet {
public:
    using Ptr = std::shared_ptr<VkPixelDescriptorSet>;
    static Ptr create(const VkPixelDevice::Ptr& device,
                      const std::vector<UniformParameter::Ptr> params,
                      const VkPixelDescriptorSetLayout::Ptr& layout,
                      const VkPixelDescriptorPool::Ptr& pool,
                      int frameCount
                  ) {
        
        return std::make_shared<VkPixelDescriptorSet>(
            device,
            params,
            layout,
            pool,
            frameCount
            );
    }

    VkPixelDescriptorSet(const VkPixelDevice::Ptr &device,
                         const std::vector<UniformParameter::Ptr> params,
                         const VkPixelDescriptorSetLayout::Ptr &layout,
                         const VkPixelDescriptorPool::Ptr &pool,
                         int frameCount);
                  

    ~VkPixelDescriptorSet();

    [[nodiscard]] auto getDescriptorSet(int frameCount) const { return mDescriptorSets[frameCount]; }

private:
    std::vector<VkDescriptorSet> mDescriptorSets{};
    VkPixelDevice::Ptr mDevice{ nullptr };
};
}

#endif /* VkPixelDescriptorSet_hpp */
