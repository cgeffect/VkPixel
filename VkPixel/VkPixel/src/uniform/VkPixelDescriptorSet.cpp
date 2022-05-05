//
//  VkPixelDescriptorSet.cpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#include "VkPixelDescriptorSet.h"

namespace vkpixel {

/*
描述符由描述符集 descriptor set 对象组成。 这些对象包含了一组描述符的存储。 描述符集会把给定的资源（例如 uniform 缓冲区，采样的图像，存储的图像等）连接到着色器，帮助它通过布局绑定（使用描述符集布局定义的）来读取和解释传入的资源数据。 例如，使用描述符将资源（如图像纹理，采样器和缓冲区）绑定到着色器。

描述符是不透明的对象，并定义了与着色器进行通信的协议；在幕后，它提供了一种静默的机制，借助位置绑定将资源内存与着色器相关联。*/
VkPixelDescriptorSet::VkPixelDescriptorSet(const VkPixelDevice::Ptr& device,
                                           const std::vector<UniformParameter::Ptr> params,
                                           const VkPixelDescriptorSetLayout::Ptr& layout,
                                           const VkPixelDescriptorPool::Ptr& pool,
                                           int frameCount
) {
    mDevice = device;

    //初始化layouts, 每一个值都是VkDescriptorSetLayout, VkDescriptorSetLayout里面包含三个uniform的信息
    std::vector<VkDescriptorSetLayout> layouts(frameCount, layout->getLayout());

    //alloc VkDescriptorSet
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool->getPool(); //从哪个descriptorPool创建
    allocInfo.descriptorSetCount = frameCount; //创建几个descriptor
    allocInfo.pSetLayouts = layouts.data();//

    //创建VkDescriptorSet
    mDescriptorSets.resize(frameCount);
    //创建VkDescriptorSet数组, 放在mDescriptorSets, 这里为什么传入&mDescriptorSets[0]就会把三个VkDescriptorSet都创建出来
    if (vkAllocateDescriptorSets(mDevice->getDevice(), &allocInfo, &mDescriptorSets[0]) != VK_SUCCESS) {
        throw std::runtime_error("Error: failed to allocate descriptor sets");
    }

    
    for (int i = 0; i < frameCount; ++i) {
        //对每个DescriptorSet，我们需要把params里面的描述信息，写入其中, 描述 Descriptor 和 Set 的关系：
        std::vector<VkWriteDescriptorSet> descriptorSetWrites{};
        for (const auto& param : params) {
            //VkWriteDescriptorSet指定binding、set或数组索引号, 根据这个索引号把数据传递到buffer、uniform、sampler2D
            VkWriteDescriptorSet descriptorSetWrite{};
            descriptorSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorSetWrite.dstSet = mDescriptorSets[i];//目标 Descriptor Set。
            descriptorSetWrite.dstArrayElement = 0;//由于资源描述可以是数组，这里需要指定一个需要更新的数组下标。如果不是数组, 则设置0
            descriptorSetWrite.descriptorType = param->mDescriptorType;//描述类型。传入一个VkDescriptorType枚举成员
            descriptorSetWrite.descriptorCount = param->mCount;//从下标开始更新几个数组元素。
            descriptorSetWrite.dstBinding = param->mBinding;//这个资源描述在资源描述集中的绑定。需要与 Layout 中声明的绑定相同。
            
            if (param->mDescriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                //传递attribute信息到顶点着色器, VkDescriptorBufferInfo包含矩阵信息
                descriptorSetWrite.pBufferInfo = &param->mBuffers[i]->getBufferInfo();
            }

            if (param->mDescriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
                //传递纹理数据到片元着色器,VkDescriptorImageInfo包含VkImage和VkSampler
                descriptorSetWrite.pImageInfo = &param->mTexture->getImageInfo();
            }

            descriptorSetWrites.push_back(descriptorSetWrite);
        }

        //更新VkDescriptorSet数组信息
        /*
         不要在“渲染时”更新 descriptor sets，即不要在 vkBeginCommandBuffer 到同步机制告诉我们 Command Buffer 已执行完毕（注意不是 vkQueueSubmit），否则会因为没有和绘制指令同步导致数据错误，产生未定义行为。
         */
        vkUpdateDescriptorSets(mDevice->getDevice(), static_cast<uint32_t>(descriptorSetWrites.size()), descriptorSetWrites.data(), 0, nullptr);
    }
}

VkPixelDescriptorSet::~VkPixelDescriptorSet() {
    
}

}
