//
//  VkApp.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkApp_hpp
#define VkApp_hpp

#include <stdio.h>

#include "src/common/VkPixelBase.h"
#include "src/device/VkPixelInstace.h"
#include "src/device/VkPixelDevice.h"
#include "src/window/VkPixelWindow.h"
#include "src/window/VkPixelSurface.h"
#include "src/device/VkPixelSwapChain.h"
#include "src/texture/VkPixelShader.h"
#include "src/pipeline/VkPixelPipeline.h"
#include "src/pipeline/VkPixelRenderPass.h"
#include "src/cmdbuf/VkPixelCommandPool.h"
#include "src/cmdbuf/VkPixelCommandBuffer.h"
#include "src/common/VkPxielSemaphore.h"
#include "src/common/VkPixelFence.h"
#include "src/cmdbuf/VkPixelBuffer.h"
#include "src/uniform/VkPixelDescriptorSetLayout.h"
#include "src/uniform/VkPixelDescriptorPool.h"
#include "src/uniform/VkPixelDescriptorSet.h"
#include "src/uniform/VkPixelDescriptor.h"
#include "src/uniform/VkPixelUniform.h"

#include "src/texture/VkPixelModel.h"

//1 定义UniformBuffer描述符,定义到buffer类里面
//2 区分model专用的uniform parameters以及全局公用的uniform parameter
namespace vkpixel {

class VkApplication {
public:
    VkApplication() = default;

    ~VkApplication() = default;

    void run();

private:
    void initWindow();

    void initVulkan();

    void mainLoop();

    void render();

    void cleanUp();

private:
    void createPipeline();
    void createRenderPass();
    void createCommandBuffers();
    void createSyncObjects();

    //重建交换链:  当窗口大小发生变化的时候，交换链也要发生变化，Frame View Pipeline RenderPass Sync
    void recreateSwapChain();

    void cleanupSwapChain();

private:
    unsigned int mWidth{ 800 };
    unsigned int mHeight{ 600 };

private:
    int mCurrentFrame{ 0 };
    VkPixelWindow::Ptr mWindow{ nullptr };
    VkPixelInstace::Ptr mInstance{ nullptr };
    VkPixelDevice::Ptr mDevice{ nullptr };
    VkPixelSurface::Ptr mSurface{ nullptr };
    VkPixelSwapChain::Ptr mSwapChain{ nullptr };
    VkPixelPipeline::Ptr mPipeline{ nullptr };
    VkPixelRenderPass::Ptr mRenderPass{ nullptr };
    VkPixelCommandPool::Ptr mCommandPool{ nullptr };

    std::vector<VkPixelCommandBuffer::Ptr> mCommandBuffers{};

    std::vector<VkPxielSemaphore::Ptr> mImageAvailableSemaphores{};
    std::vector<VkPxielSemaphore::Ptr> mRenderFinishedSemaphores{};
    std::vector<VkPixelFence::Ptr> inFlightFences{};//多帧渲染

    VkPixelUniform::Ptr mUniformManager{ nullptr };

    VkPixelModel::Ptr    mModel{ nullptr };
    VPMatrices    mVPMatrices;
};
}

#endif /* VkApp_hpp */
