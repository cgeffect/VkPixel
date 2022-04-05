//
//  Instance.hpp
//  vulkan
//
//  Created by Jason on 2022/4/4.
//

#ifndef Instance_hpp
#define Instance_hpp

#include <stdio.h>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

namespace vk {
class Instance {
public:
    using Ptr = std::shared_ptr<Instance>;
    static Ptr create() { return std::make_shared<Instance>(); }

    Instance();

    ~Instance();

private:
    VkInstance mInstance{ VK_NULL_HANDLE };
};
};
#endif /* Instance_hpp */
