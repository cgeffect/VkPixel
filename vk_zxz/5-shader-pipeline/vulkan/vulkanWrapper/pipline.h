#pragma once

#include "../base.h"
#include "device.h"
#include "shader.h"

namespace FF::Wrapper {

	class Pipeline {
	public:
		using Ptr = std::shared_ptr<Pipeline>;
		static Ptr create(const Device::Ptr& device) { return std::make_shared<Pipeline>(device); }

		Pipeline(const Device::Ptr &device);

		~Pipeline();

		void setShaderGroup(const std::vector<Shader::Ptr> &shaderGroup);

		void setViewports(const std::vector<VkViewport>& viewports) { mViewports = viewports; }

		void setScissors(const std::vector<VkRect2D>& scissors) { mScissors = scissors; }

		void pushBlendAttachment(const VkPipelineColorBlendAttachmentState& blendAttachment) { 
			mBlendAttachmentStates.push_back(blendAttachment); 
		}

		void build();

	public:
        //https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Fixed_functions
        /*
         1. 顶点输入
         结构描述了将传递给顶点着色器的顶点数据的格式。它大致以两种方式描述了这一点：
         绑定：数据之间的间距以及数据是逐顶点还是逐实例（请参阅实例化）
         属性描述：传递给顶点着色器的属性的类型，从哪个绑定加载它们以及在哪个偏移量
         */
		VkPipelineVertexInputStateCreateInfo mVertexInputState{};
        /*
         2. 图元装配
         结构描述了两件事：将从顶点绘制什么样的几何图形以及是否应该启用图元重启。前者在topology成员中指定，并且可以具有以下值：
         VK_PRIMITIVE_TOPOLOGY_POINT_LIST: 来自顶点的点
         VK_PRIMITIVE_TOPOLOGY_LINE_LIST：每2个顶点的线，不重复使用
         VK_PRIMITIVE_TOPOLOGY_LINE_STRIP：每行的结束顶点用作下一行的开始顶点
         VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: 来自每 3 个顶点的三角形，不重复使用
         VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP：每个三角形的第二个和第三个顶点用作下一个三角形的前两个顶点
         通常，顶点是按顺序按索引从顶点缓冲区加载的，但是使用元素缓冲区，您可以指定要自己使用的索引。这允许您执行优化，例如重用顶点。如果您将 primitiveRestartEnable 成员设置为VK_TRUE，则可以_STRIP通过使用 0xFFFF或的特殊索引来分解拓扑模式中的线和三角形0xFFFFFFFF。
         */
		VkPipelineInputAssemblyStateCreateInfo mAssemblyState{};
        
        //3. 视口描述信息
		VkPipelineViewportStateCreateInfo mViewportState{};
        
        //4. 光栅化器
        //获取由顶点着色器中的顶点形成的几何图形，并将其转换为片段以由片段着色器着色
		VkPipelineRasterizationStateCreateInfo mRasterState{};
        
        /*5. 多重采样
         这是执行抗锯齿的方法之一。它通过将光栅化到同一像素的多个多边形的片段着色器结果组合在一起来工作。这主要发生在边缘，这也是最明显的锯齿伪影发生的地方。因为如果只有一个多边形映射到一个像素，它不需要多次运行片段着色器，所以它比简单地渲染到更高分辨率然后缩小比例要便宜得多。启用它需要启用 GPU 功能。
        */
		VkPipelineMultisampleStateCreateInfo mSampleState{};
        
        //颜色混合
		std::vector<VkPipelineColorBlendAttachmentState> mBlendAttachmentStates{};
		VkPipelineColorBlendStateCreateInfo mBlendState{};
        
        //深度和模板测试
		VkPipelineDepthStencilStateCreateInfo mDepthStencilState{};
        
        //管道布局
        VkPipelineLayoutCreateInfo mLayoutState{};

		//TODO : needs render pass and subpass index
	private:
		VkPipeline mPipeline{ VK_NULL_HANDLE };
		VkPipelineLayout mLayout{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
		std::vector<Shader::Ptr> mShaders{};

		std::vector<VkViewport> mViewports{};
		std::vector<VkRect2D> mScissors{};
	};
}
