//
//  VkPixelImage.cpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#include "VkPixelImage.h"
#include "../cmdbuf/VkPixelCommandBuffer.h"
#include "../cmdbuf/VkPixelBuffer.h"

namespace vkpixel {

VkPixelImage::VkPixelImage(const VkPixelDevice::Ptr &device,
             const int& width,
             const int& height,
             const VkFormat& format,
             const VkImageType& imageType,
             const VkImageTiling& tiling,
             const VkImageUsageFlags& usage,
             const VkSampleCountFlagBits& sample,
             const VkMemoryPropertyFlags& properties,
             const VkImageAspectFlags& aspectFlags
 ) {
            
    mDevice = device;
    mLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    mWidth = width;
    mHeight = height;

    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.extent.width = width;//图像大小，包括长宽和深度。这里直接使用之前得出的长宽。
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.format = format;//rgb rgba
    imageCreateInfo.imageType = imageType; //图像类型，可以选1D、2D、3D。这里选 VK_IMAGE_TYPE_2D。
    //linear和optimal, linear就是线性,一行一行排列(cpu端可读的模式), 相邻的像素隔了一行, optimal:相邻的元素排列在一起
    imageCreateInfo.tiling = tiling;
    imageCreateInfo.usage = usage;//colorAttachment or depthAttachm
    imageCreateInfo.samples = sample; //多重采样数量,这里只涉及到那些成为 attachments 的图像，所以这里设置为 VK_SAMPLE_COUNT_1_BIT
    imageCreateInfo.mipLevels = 1;//Mipmap 层级，这里先不使用 Mipmapping，因此传入1
    imageCreateInfo.arrayLayers = 1; //纹理数组，这里的图像不是个纹理数组，而是单个纹理，所以传入1
    //经过format和tiling之后存储数据的方式, 不同的厂商不一样, 主要是对数据进行压缩, 减少存储占用空间
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //共享模式, 不共享, 只能被一个队列使用
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    //建立图像并保存到mImage中, 一个图像对象建好了，但是这个图像并没有和任何一块存放有图像内容的内存进行绑定（即其仅仅是个标识符）。下面将对这个标识符进行实质内存的分配。
    if (vkCreateImage(mDevice->getDevice(), &imageCreateInfo, nullptr, &mImage) != VK_SUCCESS) {
        throw std::runtime_error("Error:failed to create image");
    }

    //========================
    //首先需要查询到这个图像对象所需要的内存需求
    VkMemoryRequirements memReq{};
    vkGetImageMemoryRequirements(mDevice->getDevice(), mImage, &memReq);

    //分配内存空间
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size; //具体分配大小。同样，可能和图片的理论大小不一样。

    //找到合适的内存种类来存放图像对象内存。与之前的处理方法相同，这里我们结合memRequirements.memoryTypeBits指定内存种类，同时指定 flags：VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT声明这片内存是显卡专属来找到合适的内存类型。
    allocInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits, properties);

    //创建内存mImageMemory
    if (vkAllocateMemory(mDevice->getDevice(), &allocInfo, nullptr, &mImageMemory) != VK_SUCCESS) {
        throw std::runtime_error("Error: failed to allocate memory");
    }
    //把内存和mImage绑定起来, 到这里还是仅仅创建了一块内存, 但是这块内存并没有填入数据
    vkBindImageMemory(mDevice->getDevice(), mImage, mImageMemory, 0);

    //创建imageview, 为什么要创建imageview, 相比于直接访问VkImage, 使用VkImageView来作为访问VkImage的桥梁更好, 数据是被拷贝到VkImage中,VkImageView只是一个更好的访问VKImage的桥梁
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.viewType = imageType == VK_IMAGE_TYPE_2D ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_3D;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.image = mImage;//具体纹理图像对象
    //用这张图象的哪个部分（深度/模板，MipLevel，ArrayLayer）
    imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(mDevice->getDevice(), &imageViewCreateInfo, nullptr, &mImageView) != VK_SUCCESS) {
        throw std::runtime_error("Error: failed to create image view");
    }
}

VkPixelImage::~VkPixelImage() {
    if (mImageView != VK_NULL_HANDLE) {
        vkDestroyImageView(mDevice->getDevice(), mImageView, nullptr);
    }

    if (mImageMemory != VK_NULL_HANDLE) {
        vkFreeMemory(mDevice->getDevice(), mImageMemory, nullptr);
    }

    if (mImage != VK_NULL_HANDLE) {
        vkDestroyImage(mDevice->getDevice(), mImage, nullptr);
    }
}

uint32_t VkPixelImage::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(mDevice->getPhysicalDevice(), &memProps);

    //0x001 | 0x100 = 0x101  i = 0 第i个对应类型就是  1 << i 1   i = 1 0x010
    for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
        if ((typeFilter & (1 << i)) && ((memProps.memoryTypes[i].propertyFlags & properties) == properties)) {
            return i;
        }
    }

    throw std::runtime_error("Error: cannot find the property memory type!");
}

//图片格式转换
void VkPixelImage::setImageLayout(VkImageLayout newLayout,
                           VkPipelineStageFlags srcStageMask,
                           VkPipelineStageFlags dstStageMask,
                           VkImageSubresourceRange subresrouceRange,
                           const VkPixelCommandPool::Ptr& commandPool
                       ) {
    
    /*
     oldLayout：原来的图像布局，即生产者提供的布局。如果我们不需要管生产者的具体布局则指定VK_IMAGE_LAYOUT_UNDEFINED。
     newLayout：消费者需要的布局。
     srcQueueFamilyIndex 和 dstQueueFamilyIndex：队列家族的转移。不跨队列转换则指定VK_QUEUE_FAMILY_IGNORED。
     image：相关的图像。
     subresourceRange.aspectMask：需要考虑这张图像的哪个部分（颜色、深度、模板等），详见 VkImageAspectFlagBits。
     srcAccessMask 和 dstAccessMask：生产者和消费者转移的元素。
     */
    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.oldLayout = mLayout;
    imageMemoryBarrier.newLayout = newLayout;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.image = mImage;
    imageMemoryBarrier.subresourceRange = subresrouceRange;

    switch (mLayout)
    {
        //如果是无定义layout，说明图片刚被创建，上方一定没有任何操作，所以上方是一个虚拟的依赖
        //所以不关心上一个阶段的任何操作, 就是无依赖
    case VK_IMAGE_LAYOUT_UNDEFINED:
        imageMemoryBarrier.srcAccessMask = 0;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    default:
        break;
    }

    switch (newLayout)
    {
        //如果目标是，将图片转换成为一83个复制操作的目标图片/内存，那么被阻塞的操作一定是写入操作
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
        //如果目标是，将图片转换成为一个适合被作为纹理的格式，那么被阻塞的操作一定是，读取
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;
    default:
        break;
    }

    mLayout = newLayout;

    //image拷贝
    auto commandBuffer = VkPixelCommandBuffer::create(mDevice, commandPool);
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    commandBuffer->transferImageLayout(imageMemoryBarrier, srcStageMask, dstStageMask);
    commandBuffer->end();

    commandBuffer->submitSync(mDevice->getGraphicQueue());
}

//image data 复制到buffer里
void VkPixelImage::fillImageData(size_t size, void* pData, const VkPixelCommandPool::Ptr& commandPool) {
    assert(pData);
    assert(size);

    //把pData包装成VkBuffer
    auto stageBuffer = VkPixelBuffer::createStageBuffer(mDevice, size, pData);

    auto commandBuffer = VkPixelCommandBuffer::create(mDevice, commandPool);
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    //再把VkBuffer包装成VkImage
    commandBuffer->copyBufferToImage(stageBuffer->getBuffer(), mImage, mLayout, (uint32_t)mWidth, (uint32_t)mHeight);
    commandBuffer->end();

    commandBuffer->submitSync(mDevice->getGraphicQueue());
}
}
