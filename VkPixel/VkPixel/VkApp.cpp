//
//  VkApp.cpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#include "VkApp.h"

namespace vkpixel {

void VkApp::run() {
    initWindow();
    mainLoop();
    
    destroy();
}

void VkApp::initWindow() {
    mWindow = VkPixelWindow::create(mWidth, mHeight);
}
void VkApp::mainLoop() {
    while (!mWindow->shouldClose()) {
        mWindow->pollEvents();
    }
}


void VkApp::initVulkan() {
    
}

void VkApp::render() {
    
}

void VkApp::destroy() {
    

}

}
