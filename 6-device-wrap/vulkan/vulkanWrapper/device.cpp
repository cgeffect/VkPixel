#include "device.h"

namespace FF::Wrapper {

	Device::Device(Instance::Ptr instance) {
		mInstance = instance;
		pickPhysicalDevice();
		initQueueFamilies(mPhysicalDevice);
		createLogicalDevice();
	}

	Device::~Device() {
		vkDestroyDevice(mDevice, nullptr);
		mInstance.reset();
	}

	void Device::pickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(mInstance->getInstance(), &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("Error:failed to enumeratePhysicalDevice");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(mInstance->getInstance(), &deviceCount, devices.data());

        //从小到达排序, key最大的值在尾部
		std::multimap<int, VkPhysicalDevice> candidates;
		for (const auto& device : devices) {
			int score = rateDevice(device);
			candidates.insert(std::make_pair(score, device));
		}

        if (candidates.rbegin()->first > 0) {
            if (isDeviceSuitable(candidates.rbegin()->second)) {
                mPhysicalDevice = candidates.rbegin()->second;
            }
        } else {
            if (isDeviceSuitable(candidates.rbegin()->second)) {
                mPhysicalDevice = candidates.rbegin()->second;
            }
        }

		if (mPhysicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("Error:failed to get physical device");
		}
	}

	int Device::rateDevice(VkPhysicalDevice device) {
		int score = 0;

		//设备名称 类型 支持vulkan的版本
		VkPhysicalDeviceProperties  deviceProp;
		vkGetPhysicalDeviceProperties(device, &deviceProp);

		//纹理压缩 浮点数运算特性 多视口渲染
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        //如果是独立显卡
		if (deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score += 1000;
		}

		score += deviceProp.limits.maxImageDimension2D;

        //是否支持几何着色器
		if (!deviceFeatures.geometryShader) {
			return 0;
		}

		return score;
	}

	bool Device::isDeviceSuitable(VkPhysicalDevice device) {
		//设备名称 类型 支持vulkan的版本
		VkPhysicalDeviceProperties  deviceProp;
		vkGetPhysicalDeviceProperties(device, &deviceProp);

		//纹理压缩 浮点数运算特性 多视口渲染
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        //VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU 独立显卡
        //VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU集成显卡
//		bool isSuitable = deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
        bool integrated = deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
        std::cout << deviceProp.deviceName << std::endl;
        return integrated;
	}

    //找到具体图形功能的队列族
	void Device::initQueueFamilies(VkPhysicalDevice device) {
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;//queueFamilies内部存储也是按照0-n储存的
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
				mGraphicQueueFamily = i;
			}

			if (mGraphicQueueFamily.has_value()) {
				break;
			}

			++i;
		}
	}

	void Device::createLogicalDevice() {
		//填写创建信息
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = mGraphicQueueFamily.value();
		queueCreateInfo.queueCount = 1;

		float queuePriority = 1.0;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		//填写逻辑设备创建信息
		VkPhysicalDeviceFeatures deviceFeatures = {};

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		deviceCreateInfo.enabledExtensionCount = 0;

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
	}
}
