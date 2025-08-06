#include "application.h"

namespace FF {

	void Application::run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanUp();
	}

	void Application::initWindow() {
		mWindow = Wrapper::Window::create(WIDTH, HEIGHT);
	}

	void Application::initVulkan() {
        //创建vk实例
		mInstance = Wrapper::Instance::create(true);
        
        //创建vksurface
		mSurface = Wrapper::WindowSurface::create(mInstance, mWindow);

        //创建vk设备
		mDevice = Wrapper::Device::create(mInstance, mSurface);
	}

	void Application::mainLoop() {
		while (!mWindow->shouldClose()) {
			mWindow->pollEvents();
		}
	}

	void Application::cleanUp() {
		mDevice.reset();
		mSurface.reset();
		mInstance.reset();
		mWindow.reset();
	}
}
