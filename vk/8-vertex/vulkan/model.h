#pragma once
#include "base.h"
#include "vulkanWrapper/buffer.h"
#include "vulkanWrapper/device.h"

namespace FF {

	struct Vertex {
		glm::vec3 mPosition;
		glm::vec3 mColor;
	};

	class Model {
	public:
		using Ptr = std::shared_ptr<Model>;
		static Ptr create(const Wrapper::Device::Ptr& device) { return std::make_shared<Model>(device); }

		Model(const Wrapper::Device::Ptr &device) {
			/*mDatas = {
				{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
				{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
				{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
			};*/

			mPositions = {
				0.0f, -0.5f, 0.0f, //vulkan的y轴是朝下的, -0.5代表沿着y轴, 从0点向上
				0.5f, 0.5f, 0.0f,
				-0.5f, 0.5f, 0.0f
			};

			mColors = {
				1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f
			};

			mIndexDatas = { 0, 1, 2};

			//mVertexBuffer = Wrapper::Buffer::createVertexBuffer(device, mDatas.size() * sizeof(Vertex), mDatas.data());
            
            //顶点buffer
			mPositionBuffer = Wrapper::Buffer::createVertexBuffer(device, mPositions.size() * sizeof(float), mPositions.data());

            //颜色buffer
			mColorBuffer = Wrapper::Buffer::createVertexBuffer(device, mColors.size() * sizeof(float), mColors.data());

            //索引buffer
			mIndexBuffer = Wrapper::Buffer::createIndexBuffer(device, mIndexDatas.size() * sizeof(float), mIndexDatas.data());
		}

		~Model() {}

		//顶点数组buffer相关的信息
		std::vector<VkVertexInputBindingDescription> getVertexInputBindingDescriptions() {
			std::vector<VkVertexInputBindingDescription> bindingDes{};
			/*bindingDes.resize(1);

			bindingDes[0].binding = 0;
			bindingDes[0].stride = sizeof(Vertex);
			bindingDes[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;*/

			bindingDes.resize(2);

			bindingDes[0].binding = 0;
			bindingDes[0].stride = sizeof(float) * 3;
			bindingDes[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			bindingDes[1].binding = 1;
			bindingDes[1].stride = sizeof(float) * 3;
			bindingDes[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDes;
		}

		//Attribute相关信息，与VertexShader里面的location相关, 定义与顶点着色器的接收格式
        //binding表示获取那个数据, 与VkVertexInputBindingDescription.binding对应
        //location表示要把binding的数据传递到顶点着色器的那个位置
        //format表示顶点着色的接收类型, location=0的是vec3, 对应的是VK_FORMAT_R32G32B32_SFLOAT
        //offset表示读取数据是是否需要偏移
		std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
			std::vector<VkVertexInputAttributeDescription> attributeDes{};
			attributeDes.resize(2);

			attributeDes[0].binding = 0;
			attributeDes[0].location = 0;
			attributeDes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			//attributeDes[0].offset = offsetof(Vertex, mPosition);
			attributeDes[0].offset = 0;

			//attributeDes[1].binding = 0;
			attributeDes[1].binding = 1;
			attributeDes[1].location = 1;
			attributeDes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			//attributeDes[1].offset = offsetof(Vertex, mColor);
			attributeDes[1].offset = 0;

			return attributeDes;
		}

		//[[nodiscard]] auto getVertexBuffer() const { return mVertexBuffer; }

        //传递到顶点着色器的buffer
		[[nodiscard]] auto getVertexBuffers() const { 
			std::vector<VkBuffer> buffers{ mPositionBuffer->getBuffer(), mColorBuffer->getBuffer() };

			return buffers;
		}

        //绘制三角形的顶点索引buffer
		[[nodiscard]] auto getIndexBuffer() const { return mIndexBuffer; }
        
        //绘制三角形的顶点索引buffer的个数
		[[nodiscard]] auto getIndexCount() const { return mIndexDatas.size(); }

	private:
		//std::vector<Vertex> mDatas{};
		std::vector<float> mPositions{};
		std::vector<float> mColors{};
		std::vector<unsigned int> mIndexDatas{};

		//Wrapper::Buffer::Ptr mVertexBuffer{ nullptr };

		Wrapper::Buffer::Ptr mPositionBuffer{ nullptr };
		Wrapper::Buffer::Ptr mColorBuffer{ nullptr };

		Wrapper::Buffer::Ptr mIndexBuffer{ nullptr };
	};
}
