#include "uniformManager.h"

UniformManager::UniformManager() {

}

UniformManager::~UniformManager() {

}

void UniformManager::init(const Wrapper::Device::Ptr& device, const Wrapper::CommandPool::Ptr& commandPool, int frameCount) {
	mDevice = device;

    //uniform参数0, VPMatrices, 只设置参数类型, 不更新数据, 更新数据在update函数
	auto vpParam = Wrapper::UniformParameter::create();
	vpParam->mBinding = 0;
	vpParam->mCount = 1;
	vpParam->mDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vpParam->mSize = sizeof(VPMatrices);
	vpParam->mStage = VK_SHADER_STAGE_VERTEX_BIT;
	//swapchain队列里的每一个frame都要设置一个buffer?
	for (int i = 0; i < frameCount; ++i) {
		auto buffer = Wrapper::Buffer::createUniformBuffer(device, vpParam->mSize, nullptr);
		vpParam->mBuffers.push_back(buffer);
	}
	mUniformParams.push_back(vpParam);

    //uniform参数1, ObjectUniform
	auto objectParam = Wrapper::UniformParameter::create();
	objectParam->mBinding = 1;
	objectParam->mCount = 1;
	objectParam->mDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	objectParam->mSize = sizeof(ObjectUniform);
	objectParam->mStage = VK_SHADER_STAGE_VERTEX_BIT;

	for (int i = 0; i < frameCount; ++i) {
		auto buffer = Wrapper::Buffer::createUniformBuffer(device, objectParam->mSize, nullptr);
		objectParam->mBuffers.push_back(buffer);
	}
	mUniformParams.push_back(objectParam);

    //绑定采样器
	auto textureParam = Wrapper::UniformParameter::create();
    //layout(binding = 2) uniform sampler2D texSampler;
	textureParam->mBinding = 2;
	textureParam->mCount = 1;
	textureParam->mDescriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureParam->mStage = VK_SHADER_STAGE_FRAGMENT_BIT;
	textureParam->mTexture = Texture::create(mDevice, commandPool, "/Users/jason/Jason/project/vulkan-tutorial/Vulkan/vulkan/assets/bgra.jpg");
	mUniformParams.push_back(textureParam);

    /*
     所有的 Descriptor 大致上分为三类：采样器、图像和通用数据缓冲（UBO, SSBO），而区别就在于用途和随之而来的性能优化。

     每一个 shader 可能会用不止一个 descriptor，将所有 descriptors 打包起来便被叫做 descriptor set（本章节之后将会简称为 set），称为资源描述符集。在渲染时我们不一个一个发资源描述，而是一下发一整个描述符集给流水线传入 shader。同时，每一个描述符集都配套有一个描述符集布局 Descriptor Set Layout，可以理解为资源的布局、接口、协议，就像一个集装箱会配上一个内容清单一样，来说明这一个资源描述集中的资源描述个数和类型等。
     */
    //https://www.oreilly.com/library/view/vulkan-cookbook/9781786468154/assets/B05542-08-03-1.png
    //Uniform 由描述符和描述符池进行管理。 描述符有助于将资源与着色器连接起来。 但它可能会经常变化；因此，分配是通过预先分配的描述符缓冲区（称为描述符池 descriptor pool）来执行的。
	mDescriptorSetLayout = Wrapper::DescriptorSetLayout::create(device);
	mDescriptorSetLayout->build(mUniformParams);

	mDescriptorPool = Wrapper::DescriptorPool::create(device);
	mDescriptorPool->build(mUniformParams, frameCount);

	mDescriptorSet = Wrapper::DescriptorSet::create(device, mUniformParams, mDescriptorSetLayout, mDescriptorPool, frameCount);

}

void UniformManager::update(const VPMatrices& vpMatrices, const ObjectUniform& objectUniform, const int& frameCount) {
	//update vp matrices
	mUniformParams[0]->mBuffers[frameCount]->updateBufferByMap((void*)(&vpMatrices), sizeof(VPMatrices));

	//update object uniform
	mUniformParams[1]->mBuffers[frameCount]->updateBufferByMap((void*)(&objectUniform), sizeof(ObjectUniform));
}
