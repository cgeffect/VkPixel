//
//  main.cpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#include <stdio.h>
#include <iostream>
#include "VkApp.h"

int main() {
    
    vkpixel::VkApplication app;
    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
        
    return 0;
}
