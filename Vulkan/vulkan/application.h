#pragma once

#include "base.h"
#include "vulkanWrapper/device/instance.h"
#include "vulkanWrapper/device/device.h"
#include "vulkanWrapper/window/window.h"
#include "vulkanWrapper/window/windowSurface.h"
#include "swapChain.h"
#include "texture/shader.h"
#include "pipeline.h"
#include "renderPass.h"
#include "commandPool.h"
#include "commandBuffer.h"
#include "vulkanWrapper/semaphore/semaphore.h"
#include "vulkanWrapper/semaphore/fence.h"
#include "buffer.h"
#include "vulkanWrapper/uniform/descriptorSetLayout.h"
#include "vulkanWrapper/uniform/descriptorPool.h"
#include "vulkanWrapper/uniform/descriptorSet.h"
#include "vulkanWrapper/uniform/description.h"
#include "uniformManager.h"

#include "model.h"

namespace FF {
	//1 ����UniformBuffer������,���嵽buffer������
	//2 ����modelר�õ�uniform parameters�Լ�ȫ�ֹ��õ�uniform parameter

	class Application {
	public:
		Application() = default;

		~Application() = default;

		void run();

	private:
		void initWindow();

		void initVulkan();

		void mainLoop();

		void render();

		void cleanUp();

	private:
		void createPipeline();
		void createRenderPass();
		void createCommandBuffers();
		void createSyncObjects();

		//�ؽ�������:  �����ڴ�С�����仯��ʱ�򣬽�����ҲҪ�����仯��Frame View Pipeline RenderPass Sync
		void recreateSwapChain();

		void cleanupSwapChain();

	private:
		unsigned int mWidth{ 800 };
		unsigned int mHeight{ 600 };

	private:
		int mCurrentFrame{ 0 };
		Wrapper::Window::Ptr mWindow{ nullptr };
		Wrapper::Instance::Ptr mInstance{ nullptr };
		Wrapper::Device::Ptr mDevice{ nullptr };
		Wrapper::WindowSurface::Ptr mSurface{ nullptr };
		Wrapper::SwapChain::Ptr mSwapChain{ nullptr };
		Wrapper::Pipeline::Ptr mPipeline{ nullptr };
		Wrapper::RenderPass::Ptr mRenderPass{ nullptr };
		Wrapper::CommandPool::Ptr mCommandPool{ nullptr };

		std::vector<Wrapper::CommandBuffer::Ptr> mCommandBuffers{};

		std::vector<Wrapper::Semaphore::Ptr> mImageAvailableSemaphores{};
		std::vector<Wrapper::Semaphore::Ptr> mRenderFinishedSemaphores{};
		std::vector<Wrapper::Fence::Ptr> mFences{};

		UniformManager::Ptr mUniformManager{ nullptr };

		Model::Ptr	mModel{ nullptr };
		VPMatrices	mVPMatrices;
	};
}
