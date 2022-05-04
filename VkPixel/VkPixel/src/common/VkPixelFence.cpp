//
//  VkPixelFence.cpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#include "VkPixelFence.h"

namespace vkpixel {

VkPixelFence::VkPixelFence(const VkPixelDevice::Ptr& device, bool signaled) {
    mDevice = device;

    VkFenceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    //默认的fence为unsignal的，如果直接用会永远阻塞，需要将VkFenceCreateInfo中的flags成员变量赋值成VK_FENCE_CREATE_SIGNALED_BIT
    createInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    if (vkCreateFence(mDevice->getDevice(), &createInfo, nullptr, &mFence) != VK_SUCCESS) {
        throw std::runtime_error("Error:failed to create fence");
    }
}

VkPixelFence::~VkPixelFence() {
    if (mFence != VK_NULL_HANDLE) {
        vkDestroyFence(mDevice->getDevice(), mFence, nullptr);
    }
}

void VkPixelFence::resetFence() {
    vkResetFences(mDevice->getDevice(), 1, &mFence);
}

void VkPixelFence::waitForFences(uint64_t timeout) {
    vkWaitForFences(mDevice->getDevice(), 1, &mFence, VK_TRUE, timeout);
}
}
