//
//  VkPixelModel.hpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#ifndef VkPixelModel_hpp
#define VkPixelModel_hpp

#include <stdio.h>
#include "../common/VkPixelBase.h"
#include "../cmdbuf/VkPixelBuffer.h"
#include "../device/VkPixelDevice.h"
#include "../uniform/VkPixelDescriptorSet.h"
#include "../uniform/VkPixelDescriptor.h"

namespace vkpixel {

struct Vertex {
    glm::vec3 mPosition;
    glm::vec3 mColor;
};

class VkPixelModel {
public:
    using Ptr = std::shared_ptr<VkPixelModel>;
    static Ptr create(const VkPixelDevice::Ptr& device) {
        return std::make_shared<VkPixelModel>(device);
    }

    VkPixelModel(const VkPixelDevice::Ptr &device) {
        /*mDatas = {
            {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        };*/

        mPositions = {
            0.0f, 0.5f, 0.0f,
            0.5f, 0.0f, 0.0f,
            -0.5f, 0.0f, 0.0f,
            0.0f, -0.5f, 0.0f
        };

        mColors = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f
        };

        mUVs = {
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };

        mIndexDatas = { 0, 2, 1, 1, 2, 3};

        //mVertexBuffer = VkPixelBuffer::createVertexBuffer(device, mDatas.size() * sizeof(Vertex), mDatas.data());

        mPositionBuffer = VkPixelBuffer::createVertexBuffer(device, mPositions.size() * sizeof(float), mPositions.data());

        mColorBuffer = VkPixelBuffer::createVertexBuffer(device, mColors.size() * sizeof(float), mColors.data());

        mUVBuffer = VkPixelBuffer::createVertexBuffer(device, mUVs.size() * sizeof(float), mUVs.data());

        mIndexBuffer = VkPixelBuffer::createIndexBuffer(device, mIndexDatas.size() * sizeof(float), mIndexDatas.data());

    }

    ~VkPixelModel() {}

    //顶点数组buffer相关的信息
    std::vector<VkVertexInputBindingDescription> getVertexInputBindingDescriptions() {
        
        //解释顶点属性并绑定到着色器中
        std::vector<VkVertexInputBindingDescription> bindingDes{};
        /*bindingDes.resize(1);

        bindingDes[0].binding = 0;
        bindingDes[0].stride = sizeof(Vertex);
        bindingDes[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;*/

        bindingDes.resize(3);

        //将会绑定到哪个顶点缓冲。其编号需要和录制 Command Buffer 时 vkCmdBindVertexBuffer 传入的绑定编号相对应，以将同时传入的缓冲绑定在这个顶点属性。
        bindingDes[0].binding = 0;
        //步长
        bindingDes[0].stride = sizeof(float) * 3;
        //输入的顶点数据是逐顶点还是逐实例（instance）。这里使用逐顶点VK_VERTEX_INPUT_RATE_VERTEX
        bindingDes[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        bindingDes[1].binding = 1;
        bindingDes[1].stride = sizeof(float) * 3;
        bindingDes[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        bindingDes[2].binding = 2;
        bindingDes[2].stride = sizeof(float) * 2;
        bindingDes[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDes;
    }

    //Attribute相关信息，与VertexShader里面的location相关
    std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDes{};
        attributeDes.resize(3);

        attributeDes[0].binding = 0;
        //layout(location = x) in, x 的值是location
        //对应着 Vertex Shader 中的 location，例如在 shader 中：layout (location = 0) in vec3 inPos，这里就填 0；
        attributeDes[0].location = 0;
        //表示有三个值（R32G32B32）以及每个对应单精度浮点数
        attributeDes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        //attributeDes[0].offset = offsetof(Vertex, mPosition);
        attributeDes[0].offset = 0;

        //attributeDes[1].binding = 0;
        attributeDes[1].binding = 1;
        attributeDes[1].location = 1;
        attributeDes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        //attributeDes[1].offset = offsetof(Vertex, mColor);
        attributeDes[1].offset = 0;

        attributeDes[2].binding = 2;
        attributeDes[2].location = 2;
        attributeDes[2].format = VK_FORMAT_R32G32_SFLOAT;
        //attributeDes[0].offset = offsetof(Vertex, mPosition);
        attributeDes[2].offset = 0;

        return attributeDes;
    }

    //[[nodiscard]] auto getVertexBuffer() const { return mVertexBuffer; }

    [[nodiscard]] auto getVertexBuffers() const {
        std::vector<VkBuffer> buffers{ mPositionBuffer->getBuffer(), mColorBuffer->getBuffer(), mUVBuffer->getBuffer() };

        return buffers;
    }

    [[nodiscard]] auto getIndexBuffer() const { return mIndexBuffer; }

    [[nodiscard]] auto getIndexCount() const { return mIndexDatas.size(); }

    [[nodiscard]] auto getUniform() const { return mUniform; }

    void setModelMatrix(const glm::mat4 matrix) { mUniform.mModelMatrix = matrix; }

    void update() {
        glm::mat4 rotateMatrix = glm::mat4(1.0f);
        rotateMatrix = glm::rotate(rotateMatrix, glm::radians(mAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        mUniform.mModelMatrix = rotateMatrix;

        mAngle += 0.1f;
    }
private:
    //std::vector<Vertex> mDatas{};
    std::vector<float> mPositions{};
    std::vector<float> mColors{};
    std::vector<unsigned int> mIndexDatas{};
    std::vector<float> mUVs{};

    //VkPixelBuffer::Ptr mVertexBuffer{ nullptr };

    VkPixelBuffer::Ptr mPositionBuffer{ nullptr };
    VkPixelBuffer::Ptr mColorBuffer{ nullptr };
    VkPixelBuffer::Ptr mUVBuffer{ nullptr };

    VkPixelBuffer::Ptr mIndexBuffer{ nullptr };


    ObjectUniform mUniform;

    float mAngle{ 0.0f };

};
}

#endif /* VkPixelModel_hpp */
