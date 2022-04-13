#pragma once

#include "../base.h"
#include "device.h"

namespace FF::Wrapper {
	//思路
	/*
	* 1 attachment  
	VkAttachmentDescription：描述一个颜色或者深度模板附着的结构，他并不是一个真正的附着，而是一种单纯的描述
	VkAttachmentDescription1 VkAttachmentDescription2 VkAttachmentDescription3 VkAttachmentDescription4（深度模板），告诉外界，我要什么
	VkAttachmentReference 说明本个SubPass 需要的attachment其中的一个，这一个的数组索引id是多少，这一个附着的图片期望格式是什么
	VkSubpassDescription用来填写一个子Pass的描述结构
	VkSubpassDependency：描述不同的子流程之间的依赖关系
	*/

	class SubPass {
	public:
		SubPass();

		~SubPass();

		void addColorAttachmentReference(const VkAttachmentReference& ref);

		void addInputAttachmentReference(const VkAttachmentReference& ref);

		void setDepthStencilAttachmentReference(const VkAttachmentReference& ref);

		void buildSubPassDescription();

		[[nodiscard]] auto getSubPassDescription() const { return mSubPassDescription; }

	private:
		VkSubpassDescription mSubPassDescription{};
		std::vector<VkAttachmentReference> mColorAttachmentReferences{};
		std::vector<VkAttachmentReference> mInputAttachmentReferences{};
		VkAttachmentReference mDepthStencilAttachmentReference{};
	};

/*
 在我们完成创建管道之前，我们需要告诉Vulkan渲染时将使用的帧缓冲区附件。
 我们需要指定将有多少颜色和深度缓冲区，为每个缓冲区使用多少样本，以及在整个渲染操作中应如何处理它们的内容。所有这些信息都包装在一个渲染通道对象中*/
	class RenderPass {
	public:
		using Ptr = std::shared_ptr<RenderPass>;

		static Ptr create(const Device::Ptr& device) { return std::make_shared<RenderPass>(device); }
		
		RenderPass(const Device::Ptr &device);

		~RenderPass();

		void addSubPass(const SubPass& subpass);

		void addDependency(const VkSubpassDependency &dependency);

		void addAttachment(const VkAttachmentDescription &attachmentDes);

		void buildRenderPass();

		[[nodiscard]] auto getRenderPass() const { return mRenderPass; }

	private:
		VkRenderPass mRenderPass{VK_NULL_HANDLE};

		std::vector<SubPass> mSubPasses{};
		std::vector<VkSubpassDependency> mDependencies{};
		std::vector<VkAttachmentDescription> mAttachmentDescriptions{};

		Device::Ptr mDevice{ nullptr };
	};
}
