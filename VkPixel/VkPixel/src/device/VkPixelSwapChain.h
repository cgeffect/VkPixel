//
//  VkPixelSwapChain.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelSwapChain_hpp
#define VkPixelSwapChain_hpp

#include <stdio.h>

#include "../common/VkPixelBase.h"
#include "VkPixelDevice.h"
#include "../window/VkPixelWindow.h"
#include "../window/VkPixelSurface.h"
#include "../pipline/VkPixelRenderPass.h"

namespace vkpixel {

//swapChain就是前后缓冲区
struct SwapChainSupportInfo {
    VkSurfaceCapabilitiesKHR mCapabilities;
    std::vector<VkSurfaceFormatKHR> mFormats;
    std::vector<VkPresentModeKHR> mPresentModes;
};

class VkPixelSwapChain {
public:
    using Ptr = std::shared_ptr<VkPixelSwapChain>;
    static Ptr create(const VkPixelDevice::Ptr& device, const VkPixelWindow::Ptr& window, const VkPixelSurface::Ptr& surface) {
        return std::make_shared<VkPixelSwapChain>(device, window, surface);
    }

    VkPixelSwapChain(const VkPixelDevice::Ptr &device, const VkPixelWindow::Ptr &window, const VkPixelSurface::Ptr &surface);

    ~VkPixelSwapChain();

    SwapChainSupportInfo querySwapChainSupportInfo();

    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    VkPresentModeKHR chooseSurfacePresentMode(const std::vector<VkPresentModeKHR> &availablePresenstModes);

    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR & capabilities);

    void createFrameBuffers(const VkPixelRenderPass::Ptr &renderPass);

public:
    [[nodiscard]] auto getFormat() const { return mSwapChainFormat; }

    [[nodiscard]] auto getImageCount() const { return mImageCount; }

    [[nodiscard]] auto getSwapChain() const { return mSwapChain; }

    [[nodiscard]] auto getFrameBuffer(const int index) const { return mSwapChainFrameBuffers[index]; }

    [[nodiscard]] auto getExtent() const { return mSwapChainExtent; }

private:
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels = 1);

private:
    VkSwapchainKHR mSwapChain{ VK_NULL_HANDLE };

    VkFormat  mSwapChainFormat;
    VkExtent2D mSwapChainExtent;
    uint32_t mImageCount{ 0 };

    //vkimage由swapchain创建，销毁也要交给swapchain
    std::vector<VkImage> mSwapChainImages{};

    //对图像的管理器。管理框架
    std::vector<VkImageView> mSwapChainImageViews{};

    std::vector<VkFramebuffer> mSwapChainFrameBuffers{};


    VkPixelDevice::Ptr mDevice{ nullptr };
    VkPixelWindow::Ptr mWindow{ nullptr };
    VkPixelSurface::Ptr mSurface{ nullptr };
};
}

#endif /* VkPixelSwapChain_hpp */
