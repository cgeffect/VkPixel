//
//  VkApp.cpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#include "VkApp.h"

namespace vkpixel {
VkApp::VkApp() {
    
}

VkApp::~VkApp() {
    
}

void VkApp::run() {
    initWindow();
    mainLoop();
    
    destroy();
}

void VkApp::mainLoop() {
    while (!mWindow->shouldClose()) {
        mWindow->pollEvents();
    }
}

void VkApp::destroy() {
    

}
void VkApp::initWindow() {
    mWindow = VkPixelWindow::create(mWidth, mHeight);
}
}
