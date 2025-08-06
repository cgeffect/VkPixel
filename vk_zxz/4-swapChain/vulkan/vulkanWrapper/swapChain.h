#pragma once

#include "../base.h"
#include "device.h"
#include "window.h"
#include "windowSurface.h"

/*
 Vulkan û�С�Ĭ��֡���������ĸ���������Ҫһ�������ܹ���ӵ�����ǽ���Ⱦ���Ļ�������Ȼ���ٽ����ǿ��ӻ�����Ļ�ϡ��û�����ʩ��Ϊ�������������� Vulkan ����ʽ��������������������һ���ȴ����ֵ���Ļ��ͼ����С����ǵ�Ӧ�ó��򽫻�ȡ������ͼ���Ի��Ƶ�����Ȼ���䷵�ص������С����еľ��幤����ʽ�Լ��Ӷ����г���ͼ�������ȡ���ڽ����������÷�ʽ������������һ��Ŀ����ʹͼ��ĳ�������Ļ��ˢ����ͬ����
 */
namespace FF::Wrapper {

	struct SwapChainSupportInfo {
		VkSurfaceCapabilitiesKHR mCapabilities;
		std::vector<VkSurfaceFormatKHR> mFormats;
		std::vector<VkPresentModeKHR> mPresentModes;
	};

	class SwapChain {
	public:
		using Ptr = std::shared_ptr<SwapChain>;
		static Ptr create(const Device::Ptr& device, const Window::Ptr& window, const WindowSurface::Ptr& surface) { 
			return std::make_shared<SwapChain>(device, window, surface); 
		}

		SwapChain(const Device::Ptr &device, const Window::Ptr &window, const WindowSurface::Ptr &surface);

		~SwapChain();

		SwapChainSupportInfo querySwapChainSupportInfo();

		VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

		VkPresentModeKHR chooseSurfacePresentMode(const std::vector<VkPresentModeKHR> &availablePresenstModes);

		VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR & capabilities);

	private:
		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels = 1);

	private:
		VkSwapchainKHR mSwapChain{ VK_NULL_HANDLE };

		VkFormat  mSwapChainFormat;
		VkExtent2D mSwapChainExtent;
		uint32_t mImageCount{ 0 };

		//vkimage��swapchain����������ҲҪ����swapchain
		std::vector<VkImage> mSwapChainImages{};

		//��ͼ��Ĺ�������������
		std::vector<VkImageView> mSwapChainImageViews{};


		Device::Ptr mDevice{ nullptr };
		Window::Ptr mWindow{ nullptr };
		WindowSurface::Ptr mSurface{ nullptr };
	};
}
