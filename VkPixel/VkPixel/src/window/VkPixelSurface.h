//
//  VkPixelSurface.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelSurface_hpp
#define VkPixelSurface_hpp

#include <stdio.h>
#include "../common/VkPixelBase.h"
#include "../device/VkPixelInstace.h"
#include "VkPixelWindow.h"

namespace vkpixel {

class VkPixelSurface {
public:
    using Ptr = std::shared_ptr<VkPixelSurface>;
    static Ptr create(VkPixelInstace::Ptr instance, VkPixelWindow::Ptr window) {
        return std::make_shared<VkPixelSurface>(instance, window);
    }

    VkPixelSurface(VkPixelInstace::Ptr instance, VkPixelWindow::Ptr window);

    ~VkPixelSurface();

    [[nodiscard]] auto getSurface() const { return mSurface; }

private:
    VkSurfaceKHR mSurface{ VK_NULL_HANDLE };
    VkPixelInstace::Ptr mInstance{ nullptr };
};
}
#endif /* VkPixelSurface_hpp */
