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
    VkApp();
    ~VkApp();
    
    void run();
private:
    void initWindow();

    void mainLoop();
    
    void destroy();
    
private:
    unsigned int mWidth{ 800 };
    unsigned int mHeight{ 600 };

private:
    
    VkPixelWindow::Ptr mWindow{ nullptr };
};
}
#endif /* VkApp_hpp */
