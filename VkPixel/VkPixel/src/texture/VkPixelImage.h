//
//  VkPixelImage.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelImage_hpp
#define VkPixelImage_hpp

#include <stdio.h>

#include "../common/VkPixelBase.h"
#include "../device/VkPixelDevice.h"
#include "../cmdbuf/VkPixelCommandPool.h"

namespace vkpixel {
/*
* 分析：如果我们需要做一张被用于纹理采样的图片，那么我们首先
* 需要从undefinedLayout变换成为TransferDst， 然后在数据拷贝
* 完毕之后，再转换称为ShaderReadOnly
*/

class VkPixelImage {
public:
    using Ptr = std::shared_ptr<VkPixelImage>;
    static Ptr create(const VkPixelDevice::Ptr& device,
                      const int& width,
                      const int& height,
                      const VkFormat& format,
                      const VkImageType& imageType,
                      const VkImageTiling& tiling,
                      const VkImageUsageFlags& usage,
                      const VkSampleCountFlagBits& sample,
                      const VkMemoryPropertyFlags& properties,//memory
                      const VkImageAspectFlags& aspectFlags//view
        ) {
        return std::make_shared<VkPixelImage>(device,
                                       width,
                                       height,
                                       format,
                                       imageType,
                                       tiling,
                                       usage,
                                       sample,
                                       properties,
                                       aspectFlags
                                       );
            
    }

    VkPixelImage(const VkPixelDevice::Ptr &device,
          const int& width,
          const int& height,
          const VkFormat &format,
          const VkImageType &imageType,
          const VkImageTiling &tiling,
          const VkImageUsageFlags &usage,
          const VkSampleCountFlagBits &sample,
          const VkMemoryPropertyFlags &properties,//memory
          const VkImageAspectFlags &aspectFlags//view
    );

    ~VkPixelImage();

    //此处属于便捷写法，封装性比较好，但是可以独立作为一个工具函数
    //写到Tool的类里面
    void setImageLayout(VkImageLayout newLayout,
                        VkPipelineStageFlags srcStageMask,
                        VkPipelineStageFlags dstStageMask,
                        VkImageSubresourceRange subresrouceRange,
                        const VkPixelCommandPool::Ptr & commandPool
                        );
        

    void fillImageData(size_t size, void* pData, const VkPixelCommandPool::Ptr &commandPool);

    [[nodiscard]] auto getImage() const { return mImage; }

    [[nodiscard]] auto getLayout() const { return mLayout; }

    [[nodiscard]] auto getWidth() const { return mWidth; }

    [[nodiscard]] auto getHeight() const { return mHeight; }

    [[nodiscard]] auto getImageView() const { return mImageView; }

private:
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
    size_t                mWidth{ 0 };
    size_t                mHeight{ 0 };
    VkPixelDevice::Ptr            mDevice{ nullptr };
    //cpu端的图片句柄
    VkImage                mImage{ VK_NULL_HANDLE };
    //GPU端为图片分配的显存空间
    VkDeviceMemory        mImageMemory{ VK_NULL_HANDLE };
    //image对外的访问控制器
    VkImageView            mImageView{ VK_NULL_HANDLE };
    
    //
    VkImageLayout        mLayout{VK_IMAGE_LAYOUT_UNDEFINED};
};
}

#endif /* VkPixelImage_hpp */
