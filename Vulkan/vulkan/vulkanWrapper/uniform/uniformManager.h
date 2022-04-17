#pragma once

#include "buffer.h"
#include "descriptorSetLayout.h"
#include "descriptorPool.h"
#include "descriptorSet.h"
#include "description.h"
#include "device.h"
#include "commandPool.h"
#include "base.h"

using namespace FF;

class UniformManager {
public:
	using Ptr = std::shared_ptr<UniformManager>;
	static Ptr create() { return std::make_shared<UniformManager>(); }

	UniformManager();

	~UniformManager();

	void init(const Wrapper::Device::Ptr &device, const Wrapper::CommandPool::Ptr &commandPool, int frameCount);

	void update(const VPMatrices &vpMatrices, const ObjectUniform &objectUniform, const int& frameCount);

	[[nodiscard]] auto getDescriptorLayout() const { return mDescriptorSetLayout; }

	[[nodiscard]] auto getDescriptorSet(int frameCount) const { return mDescriptorSet->getDescriptorSet(frameCount); }

private:
	Wrapper::Device::Ptr mDevice{ nullptr };
	std::vector<Wrapper::UniformParameter::Ptr> mUniformParams;

	Wrapper::DescriptorSetLayout::Ptr mDescriptorSetLayout{ nullptr };
	Wrapper::DescriptorPool::Ptr	mDescriptorPool{ nullptr };
	Wrapper::DescriptorSet::Ptr		mDescriptorSet{ nullptr };
};
