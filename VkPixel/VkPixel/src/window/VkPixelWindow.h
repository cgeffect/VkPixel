//
//  VkPixelWindow.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelWindow_hpp
#define VkPixelWindow_hpp

#include <stdio.h>
#include "../common/VkPixelBase.h"

namespace vkpixel {

class VkPixelWindow {
public:
    using Ptr = std::shared_ptr<VkPixelWindow>;
    static Ptr create(const int& width, const int& height) {
        return std::make_shared<VkPixelWindow>(width, height);
    }

    VkPixelWindow(const int &width, const int &height);

    ~VkPixelWindow();

    bool shouldClose();

    void pollEvents();

    [[nodiscard]] auto getWindow() const { return mWindow; }

public:
    bool mWindowResized{ false };

private:
    int mWidth{ 0 };
    int mHeight{ 0 };
    GLFWwindow* mWindow{ NULL };
};
}

#endif /* VkPixelWindow_hpp */
