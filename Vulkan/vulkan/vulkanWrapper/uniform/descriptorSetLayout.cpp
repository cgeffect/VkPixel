#include "descriptorSetLayout.h"

namespace FF::Wrapper {

	DescriptorSetLayout::DescriptorSetLayout(const Device::Ptr &device) {
		mDevice = device;
	}

	DescriptorSetLayout::~DescriptorSetLayout() {
		if (mLayout != VK_NULL_HANDLE) {
			vkDestroyDescriptorSetLayout(mDevice->getDevice(), mLayout, nullptr);
		}
	}

	void DescriptorSetLayout::build(const std::vector<UniformParameter::Ptr>& params) {
		mParams = params;

		if (mLayout != VK_NULL_HANDLE) {
			vkDestroyDescriptorSetLayout(mDevice->getDevice(), mLayout, nullptr);
		}

		std::vector<VkDescriptorSetLayoutBinding> layoutBindings{};

		for (const auto& param : mParams) {
			VkDescriptorSetLayoutBinding layoutBinding{};
			layoutBinding.descriptorType = param->mDescriptorType;
			layoutBinding.binding = param->mBinding;
			layoutBinding.stageFlags = param->mStage;
			layoutBinding.descriptorCount = param->mCount;
            layoutBindings.push_back(layoutBinding);
		}

        /*
         描述符集布局(VkDescriptorSetLayout)就是零个或多个描述符(VkDescriptorSet)绑定的集合。 它提供了一个接口来读取着色器中指定位置的资源。 每个描述符绑定都有一个特殊的类型，表示它正在处理的资源类别，该绑定中的描述符数量，采样器描述符的数组，以及与其相关联的相应着色器阶段， 这些元数据信息在 VkDescriptorSetLayoutBinding 中指定。 */
		VkDescriptorSetLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
		createInfo.pBindings = layoutBindings.data();

		if (vkCreateDescriptorSetLayout(mDevice->getDevice(), &createInfo, nullptr, &mLayout) != VK_SUCCESS) {
			throw std::runtime_error("Error: failed to create descriptor set layout");
		}
	}

}
