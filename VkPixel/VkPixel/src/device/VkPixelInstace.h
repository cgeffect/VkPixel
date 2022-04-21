//
//  VkPixelInstace.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelInstace_hpp
#define VkPixelInstace_hpp

#include <stdio.h>
#include <iostream>
#include "../common/VkPixelBase.h"

namespace vkpixel {
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

class VkPixelInstace {
public:
    using Ptr = std::shared_ptr<VkPixelInstace>;
    static Ptr create(bool enableValidationLayer) { return std::make_shared<VkPixelInstace>(enableValidationLayer); }

    VkPixelInstace(bool enableValidationLayer);

    ~VkPixelInstace();

    void printAvailableExtensions();

    std::vector<const char*> getRequiredExtensions();

    //layers
    bool checkValidationLayerSupport();
    void setupDebugger();

    [[nodiscard]] VkInstance getInstance() const { return mInstance; }

    [[nodiscard]] bool getEnableValidationLayer() const { return mEnableValidationLayer; }

private:
    VkInstance mInstance{ VK_NULL_HANDLE };
    bool mEnableValidationLayer{ false };
    VkDebugUtilsMessengerEXT mDebugger{ VK_NULL_HANDLE };
};
}

#endif /* VkPixelInstace_hpp */
