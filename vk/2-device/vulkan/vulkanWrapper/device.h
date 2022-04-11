#pragma once

#include "../base.h"
#include "instance.h"

namespace FF::Wrapper {

	class Device {
	public:
		using Ptr = std::shared_ptr<Device>;
		static Ptr create(Instance::Ptr instance) { return std::make_shared<Device>(instance); }

		Device(Instance::Ptr instance);

		~Device();

		void pickPhysicalDevice();

		int rateDevice(VkPhysicalDevice device);

		bool isDeviceSuitable(VkPhysicalDevice device);

		void initQueueFamilies(VkPhysicalDevice device);

		void createLogicalDevice();
		
	private:
		VkPhysicalDevice mPhysicalDevice{ VK_NULL_HANDLE };
		Instance::Ptr mInstance{ nullptr };

		//�洢��ǰ��Ⱦ����������id
		std::optional<uint32_t> mGraphicQueueFamily;
		VkQueue	mGraphicQueue{ VK_NULL_HANDLE };

		//�߼��豸
		VkDevice mDevice{ VK_NULL_HANDLE };
	};
}