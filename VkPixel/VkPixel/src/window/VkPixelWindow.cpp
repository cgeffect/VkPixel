//
//  VkPixelWindow.cpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#include "VkPixelWindow.h"

namespace vkpixel {
    
static void windowResized(GLFWwindow* window, int width, int height) {
    auto pUserData = reinterpret_cast<VkPixelWindow*>(glfwGetWindowUserPointer(window));
    pUserData->mWindowResized = true;
}

VkPixelWindow::VkPixelWindow(const int& width, const int& height) {
    mWidth = width;
    mHeight = height;

    glfwInit();

    //设置环境，关掉opengl API
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    mWindow = glfwCreateWindow(mWidth, mHeight, "vulkan window", nullptr, nullptr);
    if (!mWindow) {
        std::cerr << "Error: failed to create window" << std::endl;
    }

    glfwSetWindowUserPointer(mWindow, this);
    glfwSetFramebufferSizeCallback(mWindow, windowResized);
}

VkPixelWindow::~VkPixelWindow() {
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

bool VkPixelWindow::shouldClose() {
    return glfwWindowShouldClose(mWindow);
}

void VkPixelWindow::pollEvents() {
    glfwPollEvents();
}
}
