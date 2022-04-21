//
//  VkPixelFence.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelFence_hpp
#define VkPixelFence_hpp

#include <stdio.h>

#include "../common/VkPixelBase.h"
#include "../device/VkPixelDevice.h"

namespace vkpixel {
/*
* fence是控制一次队列提交的标志，与semaphore区别，semaphore控制单一命令提交信息内的
* 不同执行阶段之间的依赖关系，semaphore无法手动用API去激发的
* fence控制一个队列（GraphicQueue）里面一次性提交的所有指令执行完毕
* 分为激发态/非激发态,并且可以进行API级别的控制
*/
class VkPixelFence {
public:
    using Ptr = std::shared_ptr<VkPixelFence>;
    static Ptr create(const VkPixelDevice::Ptr& device, bool signaled = true) {
        return std::make_shared<VkPixelFence>(device, signaled);
    }

    VkPixelFence(const VkPixelDevice::Ptr& device, bool signaled = true);

    ~VkPixelFence();

    //置为非激发态
    void resetFence();

    //调用此函数，如果fence没有被激发，那么阻塞在这里，等待激发
    void block(uint64_t timeout = UINT64_MAX);

    [[nodiscard]] auto getFence() const { return mFence; }
private:
    VkFence mFence{ VK_NULL_HANDLE };
    VkPixelDevice::Ptr mDevice{ nullptr };
};
}

#endif /* VkPixelFence_hpp */
