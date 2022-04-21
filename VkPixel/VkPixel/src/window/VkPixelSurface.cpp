//
//  VkPixelSurface.cpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#include "VkPixelSurface.h"

namespace vkpixel {

VkPixelSurface::VkPixelSurface(VkPixelInstace::Ptr instance, VkPixelWindow::Ptr window) {
    mInstance = instance;
    if (glfwCreateWindowSurface(instance->getInstance(), window->getWindow(), nullptr, &mSurface) != VK_SUCCESS) {
        throw std::runtime_error("Error: failed to create surface");
    }
}

VkPixelSurface::~VkPixelSurface() {
    vkDestroySurfaceKHR(mInstance->getInstance(), mSurface, nullptr);
    mInstance.reset();
}

}
