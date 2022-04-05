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

		//存储当前渲染任务队列族的id
		std::optional<uint32_t> mGraphicQueueFamily;
		VkQueue	mGraphicQueue{ VK_NULL_HANDLE };

		//逻辑设备
		VkDevice mDevice{ VK_NULL_HANDLE };
	};
}