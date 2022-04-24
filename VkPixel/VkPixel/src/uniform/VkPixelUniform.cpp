//
//  VkPixelUniform.cpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#include "VkPixelUniform.h"

namespace vkpixel {

VkPixelUniform::VkPixelUniform() {

}

VkPixelUniform::~VkPixelUniform() {

}

void VkPixelUniform::init(const VkPixelDevice::Ptr& device, const VkPixelCommandPool::Ptr& commandPool, int frameCount) {
    mDevice = device;

    //uniform参数0, VPMatrices, 只设置参数类型, 不更新数据, 更新数据在update函数
    auto vpParam = UniformParameter::create();
    vpParam->mBinding = 0;
    vpParam->mCount = 1;
    vpParam->mDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    vpParam->mSize = sizeof(VPMatrices);
    vpParam->mStage = VK_SHADER_STAGE_VERTEX_BIT;
    //swapchain队列里的每一个frame都要设置一个buffer?
    for (int i = 0; i < frameCount; ++i) {
        auto buffer = VkPixelBuffer::createUniformBuffer(device, vpParam->mSize, nullptr);
        vpParam->mBuffers.push_back(buffer);
    }
    mUniformParams.push_back(vpParam);

    //uniform参数1, ObjectUniform
    auto objectParam = UniformParameter::create();
    objectParam->mBinding = 1;
    objectParam->mCount = 1;
    objectParam->mDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    objectParam->mSize = sizeof(ObjectUniform);
    objectParam->mStage = VK_SHADER_STAGE_VERTEX_BIT;

    for (int i = 0; i < frameCount; ++i) {
        auto buffer = VkPixelBuffer::createUniformBuffer(device, objectParam->mSize, nullptr);
        objectParam->mBuffers.push_back(buffer);
    }
    mUniformParams.push_back(objectParam);

    //绑定采样器
    auto textureParam = UniformParameter::create();
    //layout(binding = 2) uniform sampler2D texSampler;
    textureParam->mBinding = 2;
    textureParam->mCount = 1;
    textureParam->mDescriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textureParam->mStage = VK_SHADER_STAGE_FRAGMENT_BIT;
    textureParam->mTexture = VkPixelTexture::create(mDevice, commandPool, "/Users/jason/Jason/project/vulkan-tutorial/VkPixel/VkPixel/resource/bgra.jpg");
    mUniformParams.push_back(textureParam);

    /*
     所有的 Descriptor 大致上分为三类：采样器、图像和通用数据缓冲（UBO, SSBO），而区别就在于用途和随之而来的性能优化。

     每一个 shader 可能会用不止一个 descriptor，将所有 descriptors 打包起来便被叫做 descriptor set（本章节之后将会简称为 set），称为资源描述符集。在渲染时我们不一个一个发资源描述，而是一下发一整个描述符集给流水线传入 shader。同时，每一个描述符集都配套有一个描述符集布局 Descriptor Set Layout，可以理解为资源的布局、接口、协议，就像一个集装箱会配上一个内容清单一样，来说明这一个资源描述集中的资源描述个数和类型等。
     */
    //https://www.oreilly.com/library/view/vulkan-cookbook/9781786468154/assets/B05542-08-03-1.png
    //Uniform 由描述符和描述符池进行管理。 描述符有助于将资源与着色器连接起来。 但它可能会经常变化；因此，分配是通过预先分配的描述符缓冲区（称为描述符池 descriptor pool）来执行的。
    
    /*
     Vulkan也为此提供了不同的方法。原则上，uniform数据可以通过三种方式输入：
     统一缓冲区绑定：作为DescriptorSet的一部分，这相当于OpenGL中的任意glBindBufferRange（GL_UNIFORM_BUFFER，dset.binding，dset.bufferOffset，dset.bufferSize）。CommandBuffer实际绑定的所有信息都存储在DescriptorSet本身中。
     统一缓冲区动态绑定：与上面类似，但稍后在录制CommandBuffer时能够提供bufferOffset，有点像这个伪代码：CommandBuffer->BindDescriptorSet（setNumber，descriptorSet，&offset）。从较大的缓冲区分配子分配统一缓冲区时，使用起来非常实用。
     Push Constants：PushConstants是存储在CommandBuffer中的统一值，可以从着色器访问，类似于单个全局统一缓冲区。它们提供了足够的字节来容纳一些矩阵或索引值，对原始数据的解释是着色器。您可能还记得OpenGL提供的glProgramEnvParameter提供了类似的东西。这些值使用CommandBuffer记录，之后无法更改：CommandBuffer->PushConstant（偏移、大小和数据）
     https://developer.nvidia.com/vulkan-shader-resource-binding
     */
    mDescriptorSetLayout = VkPixelDescriptorSetLayout::create(device);
    mDescriptorSetLayout->build(mUniformParams);

    mDescriptorPool = VkPixelDescriptorPool::create(device);
    mDescriptorPool->build(mUniformParams, frameCount);

    //VkDescriptorSet是和VkBuffer绑定的, 着色器通过VkDescriptorSet读取VkBuffer里的数据, VkDescriptorSet又是从VkDescriptorPool里获取的
    mDescriptorSet = VkPixelDescriptorSet::create(device, mUniformParams, mDescriptorSetLayout, mDescriptorPool, frameCount);

}

void VkPixelUniform::update(const VPMatrices& vpMatrices, const ObjectUniform& objectUniform, const int& frameCount) {
    //update vp matrices
    mUniformParams[0]->mBuffers[frameCount]->updateBufferByMap((void*)(&vpMatrices), sizeof(VPMatrices));

    //update object uniform
    mUniformParams[1]->mBuffers[frameCount]->updateBufferByMap((void*)(&objectUniform), sizeof(ObjectUniform));
}
}
