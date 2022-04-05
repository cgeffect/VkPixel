#include "application.h"

namespace FF {

    void Application::run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanUp();
    }

    void Application::initWindow() {
        glfwInit();
        
        
        //设置环境，关掉opengl API 并且禁止窗口改变大小
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        mWindow = glfwCreateWindow(WIDTH, HEIGHT, "vulkan window", nullptr, nullptr);
        if (!mWindow) {
            std::cerr << "Error: failed to create window" << std::endl;
        }
    }

    void Application::initVulkan() {
        mInstance = Wrapper::Instance::create(true);
    }

    void Application::mainLoop() {
        while (!glfwWindowShouldClose(mWindow)) {
            glfwPollEvents();
        }
    }

    void Application::cleanUp() {
        mInstance.reset();//智能指针reset使引用计数减一
        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }
}
