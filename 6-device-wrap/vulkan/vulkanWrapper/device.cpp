#include "device.h"

namespace FF::Wrapper {

	Device::Device(Instance::Ptr instance) {
		mInstance = instance;
        //���������豸
		pickPhysicalDevice();
        //��ʼ��������
		initQueueFamilies(mPhysicalDevice);
        //�����߼��豸
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

        //��С��������, key����ֵ��β��
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

    //�豸���
	int Device::rateDevice(VkPhysicalDevice device) {
		int score = 0;

		//�豸���� ���� ֧��vulkan�İ汾
		VkPhysicalDeviceProperties  deviceProp;
		vkGetPhysicalDeviceProperties(device, &deviceProp);

		//����ѹ�� �������������� ���ӿ���Ⱦ
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        //����Ƕ����Կ�
		if (deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score += 1000;
		}

		score += deviceProp.limits.maxImageDimension2D;

        //�Ƿ�֧�ּ�����ɫ��
		if (!deviceFeatures.geometryShader) {
			return 0;
		}

		return score;
	}

	bool Device::isDeviceSuitable(VkPhysicalDevice device) {
		//�豸���� ���� ֧��vulkan�İ汾
		VkPhysicalDeviceProperties  deviceProp;
		vkGetPhysicalDeviceProperties(device, &deviceProp);

		//����ѹ�� �������������� ���ӿ���Ⱦ
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        //VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU �����Կ�
        //VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU�����Կ�
//		bool isSuitable = deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
        bool integrated = deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
        std::cout << deviceProp.deviceName << std::endl;
        return integrated;
	}

    //�ҵ�����ͼ�ι��ܵĶ�����
	void Device::initQueueFamilies(VkPhysicalDevice device) {
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;//queueFamilies�ڲ��洢Ҳ�ǰ���0-n�����
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
		//��д���д�����Ϣ
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        //ָ�����ĸ������������ȡ����
		queueCreateInfo.queueFamilyIndex = mGraphicQueueFamily.value();
        printf("queueCreateInfo.queueFamilyIndex = %d\n", mGraphicQueueFamily.value());
        //Ҫ��ȡ��������
		queueCreateInfo.queueCount = 1;

        //���ö��е����ȼ�
		float queuePriority = 1.0;
		queueCreateInfo.pQueuePriorities = &queuePriority;

        //ͨ��������Ϣ����VkDeviceCreateInfo
		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
		deviceCreateInfo.queueCreateInfoCount = 1;
        VkPhysicalDeviceFeatures deviceFeatures = {};
        //�豸����, ����Ϊ0
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		deviceCreateInfo.enabledExtensionCount = 0;

		//layer��
		if (mInstance->getEnableValidationLayer()) {
			deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			deviceCreateInfo.enabledLayerCount = 0;
		}

        //���������豸
		if (vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice) != VK_SUCCESS) {
			throw std::runtime_error("Error:failed to create logical device");
		}

        //��ͼ�ι��ܵĶ����������ȡ����Ⱦͼ�εĶ���
		vkGetDeviceQueue(mDevice, mGraphicQueueFamily.value(), 0, &mGraphicQueue);
        
        printf("");
	}
}
