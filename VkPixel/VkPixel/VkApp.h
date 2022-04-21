//
//  VkApp.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkApp_hpp
#define VkApp_hpp

#include <stdio.h>
#include "src/window/VkPixelWindow.h"

namespace vkpixel {
class VkApp {
public:
    //default修饰符表示未实现
    VkApp() = default;
    ~VkApp() = default;
    
    void run();
private:
    void initWindow();

    void initVulkan();

    void mainLoop();

    void render();

    void destroy();
    
private:
    unsigned int mWidth{ 800 };
    unsigned int mHeight{ 600 };

private:
    
    VkPixelWindow::Ptr mWindow{ nullptr };
};
}
#endif /* VkApp_hpp */
