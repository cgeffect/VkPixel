//
//  VkPixelRenderPass.cpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#include "VkPixelRenderPass.h"

namespace vkpixel {

VkPixelSubPass::VkPixelSubPass() {

}

VkPixelSubPass::~VkPixelSubPass() {

}

void VkPixelSubPass::addColorAttachmentReference(const VkAttachmentReference& ref) {
    mColorAttachmentReferences.push_back(ref);
}

void VkPixelSubPass::addInputAttachmentReference(const VkAttachmentReference& ref) {
    mInputAttachmentReferences.push_back(ref);
}

void VkPixelSubPass::setDepthStencilAttachmentReference(const VkAttachmentReference& ref) {
    mDepthStencilAttachmentReference = ref;
}

void VkPixelSubPass::buildSubPassDescription() {
    if (mColorAttachmentReferences.empty()) {
        throw std::runtime_error("Error: color attachment group is empty!");
    }
    mSubPassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    mSubPassDescription.colorAttachmentCount = static_cast<uint32_t>(mColorAttachmentReferences.size());
    mSubPassDescription.pColorAttachments = mColorAttachmentReferences.data();

    mSubPassDescription.inputAttachmentCount = static_cast<uint32_t>(mInputAttachmentReferences.size());
    mSubPassDescription.pInputAttachments = mInputAttachmentReferences.data();

    mSubPassDescription.pDepthStencilAttachment = mDepthStencilAttachmentReference.layout == VK_IMAGE_LAYOUT_UNDEFINED? nullptr : &mDepthStencilAttachmentReference;
}


VkPixelRenderPass::VkPixelRenderPass(const VkPixelDevice::Ptr& device) {
    mDevice = device;
}

VkPixelRenderPass::~VkPixelRenderPass() {
    if (mRenderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(mDevice->getDevice(), mRenderPass, nullptr);
    }
}

void VkPixelRenderPass::addSubPass(const VkPixelSubPass& subpass) { mSubPasses.push_back(subpass); }

void VkPixelRenderPass::addDependency(const VkSubpassDependency& dependency) { mDependencies.push_back(dependency); }

void VkPixelRenderPass::addAttachment(const VkAttachmentDescription& attachmentDes) { mAttachmentDescriptions.push_back(attachmentDes); }

void VkPixelRenderPass::buildRenderPass() {
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
