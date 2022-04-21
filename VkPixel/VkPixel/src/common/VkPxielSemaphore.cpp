//
//  VkPxielSemaphore.cpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#include "VkPxielSemaphore.h"

namespace vkpixel {

VkPxielSemaphore::VkPxielSemaphore(const VkPixelDevice::Ptr& device) {
    mDevice = device;

    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(mDevice->getDevice(), &createInfo, nullptr, &mSemaphore) != VK_SUCCESS) {
        throw std::runtime_error("Error: failed to create Semaphore");
    }
}

VkPxielSemaphore::~VkPxielSemaphore() {
    if (mSemaphore != VK_NULL_HANDLE) {
        vkDestroySemaphore(mDevice->getDevice(), mSemaphore, nullptr);
    }
}
}
