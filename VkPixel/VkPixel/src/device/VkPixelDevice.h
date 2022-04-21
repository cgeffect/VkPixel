//
//  VkPixelDevice.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelDevice_hpp
#define VkPixelDevice_hpp

#include <stdio.h>
#include "../common/VkPixelBase.h"
#include "VkPixelInstace.h"
#include "../window/VkPixelSurface.h"

namespace vkpixel {
const std::vector<const char*> deviceRequiredExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_MAINTENANCE1_EXTENSION_NAME //开启y坐标向上扩展
};

class VkPixelDevice {
public:
    using Ptr = std::shared_ptr<VkPixelDevice>;
    static Ptr create(VkPixelInstace::Ptr instance, VkPixelSurface::Ptr surface) {
        return std::make_shared<VkPixelDevice>(instance, surface);
    }

    VkPixelDevice(VkPixelInstace::Ptr instance, VkPixelSurface::Ptr surface);

    ~VkPixelDevice();

    void pickPhysicalDevice();

    int rateDevice(VkPhysicalDevice device);

    bool isDeviceSuitable(VkPhysicalDevice device);

    void initQueueFamilies(VkPhysicalDevice device);

    void createLogicalDevice();

    bool isQueueFamilyComplete();

    [[nodiscard]] auto getDevice() const { return mDevice; }
    [[nodiscard]] auto getPhysicalDevice() const { return mPhysicalDevice; }

    [[nodiscard]] auto getGraphicQueueFamily() const { return mGraphicQueueFamily; }
    [[nodiscard]] auto getPresentQueueFamily() const { return mPresentQueueFamily; }

    [[nodiscard]] auto getGraphicQueue() const { return mGraphicQueue; }
    [[nodiscard]] auto getPresentQueue() const { return mPresentQueue; }
private:
    VkPhysicalDevice mPhysicalDevice{ VK_NULL_HANDLE };
    VkPixelInstace::Ptr mInstance{ nullptr };
    VkPixelSurface::Ptr mSurface{ nullptr };

    //存储当前渲染任务队列族的id
    std::optional<uint32_t> mGraphicQueueFamily;
    VkQueue    mGraphicQueue{ VK_NULL_HANDLE };

    std::optional<uint32_t> mPresentQueueFamily;
    VkQueue mPresentQueue{ VK_NULL_HANDLE };

    //逻辑设备
    VkDevice mDevice{ VK_NULL_HANDLE };
};
}
#endif /* VkPixelDevice_hpp */
