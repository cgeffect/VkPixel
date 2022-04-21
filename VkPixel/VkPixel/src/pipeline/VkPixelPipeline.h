//
//  VkPixelPipeline.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelPipeline_hpp
#define VkPixelPipeline_hpp

#include <stdio.h>

#include "../common/VkPixelBase.h"
#include "../device/VkPixelDevice.h"
#include "../texture/VkPixelShader.h"
#include "VkPixelRenderPass.h"

namespace vkpixel {

class VkPixelPipeline {
public:
    using Ptr = std::shared_ptr<VkPixelPipeline>;
    static Ptr create(const VkPixelDevice::Ptr& device, const VkPixelRenderPass::Ptr& renderPass) {
        return std::make_shared<VkPixelPipeline>(device, renderPass);
    }

    VkPixelPipeline(const VkPixelDevice::Ptr &device, const VkPixelRenderPass::Ptr &renderPass);

    ~VkPixelPipeline();

    void setShaderGroup(const std::vector<VkPixelShader::Ptr> &shaderGroup);

    void setViewports(const std::vector<VkViewport>& viewports) { mViewports = viewports; }

    void setScissors(const std::vector<VkRect2D>& scissors) { mScissors = scissors; }

    void pushBlendAttachment(const VkPipelineColorBlendAttachmentState& blendAttachment) {
        mBlendAttachmentStates.push_back(blendAttachment);
    }

    void build();

public:
    VkPipelineVertexInputStateCreateInfo mVertexInputState{};
    VkPipelineInputAssemblyStateCreateInfo mAssemblyState{};
    VkPipelineViewportStateCreateInfo mViewportState{};
    VkPipelineRasterizationStateCreateInfo mRasterState{};
    VkPipelineMultisampleStateCreateInfo mSampleState{};
    std::vector<VkPipelineColorBlendAttachmentState> mBlendAttachmentStates{};
    VkPipelineColorBlendStateCreateInfo mBlendState{};
    VkPipelineDepthStencilStateCreateInfo mDepthStencilState{};
    VkPipelineLayoutCreateInfo mLayoutState{};

public:
    [[nodiscard]] auto getPipeline() const { return mPipeline; }
    [[nodiscard]] auto getLayout() const { return mLayout; }

private:
    VkPipeline mPipeline{ VK_NULL_HANDLE };
    VkPipelineLayout mLayout{ VK_NULL_HANDLE };
    VkPixelDevice::Ptr mDevice{ nullptr };
    VkPixelRenderPass::Ptr mRenderPass{ nullptr };

    std::vector<VkPixelShader::Ptr> mShaders{};

    std::vector<VkViewport> mViewports{};
    std::vector<VkRect2D> mScissors{};
};
}

#endif /* VkPixelPipeline_hpp */
