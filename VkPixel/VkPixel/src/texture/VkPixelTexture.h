//
//  VkPixelTexture.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelTexture_hpp
#define VkPixelTexture_hpp

#include <stdio.h>

#include "../common/VkPixelBase.h"
#include "VkPixelImage.h"
#include "VkPixelSampler.h"
#include "../device/VkPixelDevice.h"
#include "../cmdbuf/VkPixelCmdPool.h"

namespace vkpixel {

//texture包含vkimage和vksample
class VkPixelTexture {
public:
    using Ptr = std::shared_ptr<VkPixelTexture>;
    static Ptr create(const VkPixelDevice::Ptr& device, const VkPixelCmdPool::Ptr &commandPool, const std::string& imageFilePath) {
        return std::make_shared<VkPixelTexture>(device, commandPool, imageFilePath);
    }

    VkPixelTexture(const VkPixelDevice::Ptr &device, const VkPixelCmdPool::Ptr& commandPool, const std::string &imageFilePath);

    ~VkPixelTexture();

    [[nodiscard]] auto getImage() const { return mImage; }
    
    [[nodiscard]] auto getSampler() const { return mSampler; }

    [[nodiscard]] VkDescriptorImageInfo& getImageInfo() { return mImageInfo; }

private:
    VkPixelDevice::Ptr mDevice{ nullptr };
    VkPixelImage::Ptr mImage{ nullptr };
    VkPixelSampler::Ptr mSampler{ nullptr };
    //纹理描述符
    VkDescriptorImageInfo mImageInfo{};
};

}

#endif /* VkPixelTexture_hpp */
