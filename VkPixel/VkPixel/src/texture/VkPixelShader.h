//
//  VkPixelShader.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelShader_hpp
#define VkPixelShader_hpp

#include <stdio.h>
#include "../common/VkPixelBase.h"
#include "../device/VkPixelDevice.h"

namespace vkpixel {

class VkPixelShader {
public:
    using Ptr = std::shared_ptr<VkPixelShader>;
    static Ptr create(const VkPixelDevice::Ptr& device, const std::string& fileName, VkShaderStageFlagBits shaderStage, const std::string& entryPoint) {
        return std::make_shared<VkPixelShader>(device, fileName, shaderStage, entryPoint);
    }

    VkPixelShader(const VkPixelDevice::Ptr& device, const std::string &fileName, VkShaderStageFlagBits shaderStage, const std::string &entryPoint);

    ~VkPixelShader();

    [[nodiscard]] auto getShaderStage() const { return mShaderStage; }
    [[nodiscard]] auto& getShaderEntryPoint() const { return mEntryPoint; }
    [[nodiscard]] auto getShaderModule() const { return mShaderModule; }

private:
    //着色器代码缓存的句柄
    VkShaderModule mShaderModule{ VK_NULL_HANDLE };
    VkPixelDevice::Ptr mDevice{ nullptr };
    std::string mEntryPoint;
    VkShaderStageFlagBits mShaderStage;
};
}

#endif /* VkPixelShader_hpp */
