//
//  VkPixelTexture.cpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#include "VkPixelTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace vkpixel {

VkPixelTexture::VkPixelTexture(const VkPixelDevice::Ptr &device, const VkPixelCommandPool::Ptr& commandPool, const std::string &imageFilePath) {
    
    mDevice = device;

    int texWidth, texHeight, texSize, texChannles;
    stbi_uc* pixels = stbi_load(imageFilePath.c_str(), &texWidth, &texHeight, &texChannles, STBI_rgb_alpha);

    if (!pixels) {
        throw std::runtime_error("Error: failed to read image data");
    }

    texSize = texWidth * texHeight * 4;

    mImage = VkPixelImage::create(mDevice, texWidth, texHeight,
                                    VK_FORMAT_R8G8B8A8_SRGB,
                                    VK_IMAGE_TYPE_2D,
                                    VK_IMAGE_TILING_OPTIMAL,
                                    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                    VK_SAMPLE_COUNT_1_BIT,
                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                    VK_IMAGE_ASPECT_COLOR_BIT
                                );
        
    VkImageSubresourceRange region{};
    region.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    region.baseArrayLayer = 0;
    region.layerCount = 1;

    region.baseMipLevel = 0;
    region.levelCount = 1;

    //图像布局转换, 从原始数据到创建图像对象需要一次布局转换
    //接下来将暂存缓冲里的数据拷贝到刚刚为这张图象分配的内存中，但图像数据最好不要直接从 Staging Buffer 中复制，而是要将缓冲中的格式转换成图像需要的格式
    //转换为VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL格式
    mImage->setImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           VK_PIPELINE_STAGE_TRANSFER_BIT,
                           VK_PIPELINE_STAGE_TRANSFER_BIT,
                           region,
                           commandPool);
        

    //数据可以通过VkImage句柄获取, 拷贝缓冲到图像对象
    mImage->fillImageData(texSize, (void*)pixels, commandPool);

    //图像对象到 shader 又需要一次布局转换
    //转换VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL格式
    mImage->setImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                           VK_PIPELINE_STAGE_TRANSFER_BIT,
                           VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                           region,
                           commandPool);
        
    stbi_image_free(pixels);

    //创建采样器
    mSampler = VkPixelSampler::create(mDevice);

    //组合采样器
    mImageInfo.imageLayout = mImage->getLayout();
    mImageInfo.imageView = mImage->getImageView();
    mImageInfo.sampler = mSampler->getSampler();
}

VkPixelTexture::~VkPixelTexture() {

}
}
