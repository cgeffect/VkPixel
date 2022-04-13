#include "renderPass.h"

namespace FF::Wrapper {

	SubPass::SubPass() {

	}

	SubPass::~SubPass() {

	}

	void SubPass::addColorAttachmentReference(const VkAttachmentReference& ref) {
		mColorAttachmentReferences.push_back(ref);
	}

	void SubPass::addInputAttachmentReference(const VkAttachmentReference& ref) {
		mInputAttachmentReferences.push_back(ref);
	}

    
	void SubPass::setDepthStencilAttachmentReference(const VkAttachmentReference& ref) {
		mDepthStencilAttachmentReference = ref;
	}

	void SubPass::buildSubPassDescription() {
		if (mColorAttachmentReferences.empty()) {
			throw std::runtime_error("Error: color attachment group is empty!");
		}
        //子通道绘图
		mSubPassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        //该数组中附件的索引是直接从片段着色器中使用layout(location = 0) out vec4 outColor指令引用的！
		mSubPassDescription.colorAttachmentCount = static_cast<uint32_t>(mColorAttachmentReferences.size());
		mSubPassDescription.pColorAttachments = mColorAttachmentReferences.data();

        //从着色器中读取的附件
		mSubPassDescription.inputAttachmentCount = static_cast<uint32_t>(mInputAttachmentReferences.size());
		mSubPassDescription.pInputAttachments = mInputAttachmentReferences.data();

        //深度和模板数据的附件
		mSubPassDescription.pDepthStencilAttachment = mDepthStencilAttachmentReference.layout == VK_IMAGE_LAYOUT_UNDEFINED? nullptr : &mDepthStencilAttachmentReference;
	}


    //==============================================
	RenderPass::RenderPass(const Device::Ptr& device) {
		mDevice = device;
	}

	RenderPass::~RenderPass() {
		if (mRenderPass != VK_NULL_HANDLE) {
			vkDestroyRenderPass(mDevice->getDevice(), mRenderPass, nullptr);
		}
	}

	void RenderPass::addSubPass(const SubPass& subpass) { mSubPasses.push_back(subpass); }

	void RenderPass::addDependency(const VkSubpassDependency& dependency) { mDependencies.push_back(dependency); }

	void RenderPass::addAttachment(const VkAttachmentDescription& attachmentDes) { mAttachmentDescriptions.push_back(attachmentDes); }

	void RenderPass::buildRenderPass() {
		if (mSubPasses.empty() || mAttachmentDescriptions.empty() || mDependencies.empty()) {
			throw std::runtime_error("Error: not enough elements to build renderPass");
		}

		//unwrap
		std::vector<VkSubpassDescription> subPasses{};
		for (int i = 0; i < mSubPasses.size(); ++i) {
			subPasses.push_back(mSubPasses[i].getSubPassDescription());
		}

		VkRenderPassCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

		createInfo.attachmentCount = static_cast<uint32_t>(mAttachmentDescriptions.size());
		createInfo.pAttachments = mAttachmentDescriptions.data();

		createInfo.dependencyCount = static_cast<uint32_t>(mDependencies.size());
		createInfo.pDependencies = mDependencies.data();

		createInfo.subpassCount = static_cast<uint32_t>(subPasses.size());
		createInfo.pSubpasses = subPasses.data();

		if (vkCreateRenderPass(mDevice->getDevice(), &createInfo, nullptr, &mRenderPass) != VK_SUCCESS) {
			throw std::runtime_error("Error: failed to create renderPass");
		}
	}
}
