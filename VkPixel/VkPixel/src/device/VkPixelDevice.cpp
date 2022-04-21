//
//  VkPixelDevice.cpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#include "VkPixelDevice.h"

namespace vkpixel {

VkPixelDevice::VkPixelDevice(VkPixelInstace::Ptr instance, VkPixelSurface::Ptr surface) {
    mInstance = instance;
    mSurface = surface;
    pickPhysicalDevice();
    initQueueFamilies(mPhysicalDevice);
    createLogicalDevice();
}

VkPixelDevice::~VkPixelDevice() {
    vkDestroyDevice(mDevice, nullptr);
    mSurface.reset();
    mInstance.reset();
}

void VkPixelDevice::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(mInstance->getInstance(), &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("Error:failed to enumeratePhysicalDevice");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(mInstance->getInstance(), &deviceCount, devices.data());

    std::multimap<int, VkPhysicalDevice> candidates;
    for (const auto& device : devices) {
        int score = rateDevice(device);
        candidates.insert(std::make_pair(score, device));
    }

    if (isDeviceSuitable(candidates.rbegin()->second)) {
        mPhysicalDevice = candidates.rbegin()->second;
    }

    if (mPhysicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Error:failed to get physical device");
    }
}

int VkPixelDevice::rateDevice(VkPhysicalDevice device) {
    int score = 0;

    //设备名称 类型 支持vulkan的版本
    VkPhysicalDeviceProperties  deviceProp;
    vkGetPhysicalDeviceProperties(device, &deviceProp);

    //纹理压缩 浮点数运算特性 多视口渲染
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    if (deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    score += deviceProp.limits.maxImageDimension2D;

    if (!deviceFeatures.geometryShader) {
        return 0;
    }

    return score;
}

bool VkPixelDevice::isDeviceSuitable(VkPhysicalDevice device) {
    //设备名称 类型 支持vulkan的版本
    VkPhysicalDeviceProperties  deviceProp;
    vkGetPhysicalDeviceProperties(device, &deviceProp);

    //纹理压缩 浮点数运算特性 多视口渲染
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    return deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU &&
        deviceFeatures.samplerAnisotropy;
}

void VkPixelDevice::initQueueFamilies(VkPhysicalDevice device) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
            mGraphicQueueFamily = i;
        }

        //寻找支持显示的队列族
        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface->getSurface(), &presentSupport);

        if (presentSupport) {
            mPresentQueueFamily = i;
        }

        if (isQueueFamilyComplete()) {
            break;
        }

        ++i;
    }
}

void VkPixelDevice::createLogicalDevice() {
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::set<uint32_t> queueFamilies = {mGraphicQueueFamily.value(), mPresentQueueFamily.value()};

    float queuePriority = 1.0;

    for (uint32_t queueFamily : queueFamilies) {
        //填写创建信息
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(queueCreateInfo);
    }

    //填写逻辑设备创建信息
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE; //各向异性

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceRequiredExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceRequiredExtensions.data();

    //layer层
    if (mInstance->getEnableValidationLayer()) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        deviceCreateInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice) != VK_SUCCESS) {
        throw std::runtime_error("Error:failed to create logical device");
    }

    vkGetDeviceQueue(mDevice, mGraphicQueueFamily.value(), 0, &mGraphicQueue);
    vkGetDeviceQueue(mDevice, mPresentQueueFamily.value(), 0, &mPresentQueue);
}

bool VkPixelDevice::isQueueFamilyComplete() {
    return mGraphicQueueFamily.has_value() && mPresentQueueFamily.has_value();
}
}
