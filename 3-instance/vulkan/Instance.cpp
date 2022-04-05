//
//  Instance.cpp
//  vulkan
//
//  Created by Jason on 2022/4/4.
//

#include "Instance.hpp"

namespace vk {

Instance::Instance() {
    
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "vulkanLession";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "NO ENGINE";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instCreateInfo = {};
    instCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instCreateInfo.pApplicationInfo = &appInfo;


    if (vkCreateInstance(&instCreateInfo, nullptr, &mInstance) != VK_SUCCESS) {
        throw std::runtime_error("Error:failed to create instance");
    }

}

Instance::~Instance() {
    vkDestroyInstance(mInstance, nullptr);
}

}
