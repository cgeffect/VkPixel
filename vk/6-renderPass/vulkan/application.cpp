#include "application.h"

namespace FF {

	void Application::run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanUp();
	}

	void Application::initWindow() {
		mWindow = Wrapper::Window::create(WIDTH, HEIGHT);
	}

	//1 rendePass 加入pipeline 2 生成FrameBuffer
	void Application::initVulkan() {
		mInstance = Wrapper::Instance::create(true);
        
		mSurface = Wrapper::WindowSurface::create(mInstance, mWindow);

		mDevice = Wrapper::Device::create(mInstance, mSurface);

		mSwapChain = Wrapper::SwapChain::create(mDevice, mWindow, mSurface);

		mRenderPass = Wrapper::RenderPass::create(mDevice);
		createRenderPass();

		mSwapChain->createFrameBuffers(mRenderPass);

		mPipeline = Wrapper::Pipeline::create(mDevice, mRenderPass);
		createPipeline();
	}

	void Application::createPipeline() {
		//设置视口
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)WIDTH;
		viewport.height = (float)HEIGHT;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = {0, 0};
		scissor.extent = {WIDTH, HEIGHT};

		mPipeline->setViewports({ viewport });
		mPipeline->setScissors({scissor});


		//设置shader
		std::vector<Wrapper::Shader::Ptr> shaderGroup{};

		auto shaderVertex = Wrapper::Shader::create(mDevice, "/Users/jason/Jason/project/vulkan-tutorial/vk/6-renderPass/vulkan/shaders/vs.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
		shaderGroup.push_back(shaderVertex);

		auto shaderFragment = Wrapper::Shader::create(mDevice, "/Users/jason/Jason/project/vulkan-tutorial/vk/6-renderPass/vulkan/shaders/fs.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");
		shaderGroup.push_back(shaderFragment);
		
		mPipeline->setShaderGroup(shaderGroup);

		//顶点的排布模式
		mPipeline->mVertexInputState.vertexBindingDescriptionCount = 0;
		mPipeline->mVertexInputState.pVertexBindingDescriptions = nullptr;
		mPipeline->mVertexInputState.vertexAttributeDescriptionCount = 0;
		mPipeline->mVertexInputState.pVertexAttributeDescriptions = nullptr;

		//图元装配
		mPipeline->mAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		mPipeline->mAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		mPipeline->mAssemblyState.primitiveRestartEnable = VK_FALSE;

		//光栅化设置
		mPipeline->mRasterState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		mPipeline->mRasterState.polygonMode = VK_POLYGON_MODE_FILL;//其他模式需要开启gpu特性
		mPipeline->mRasterState.lineWidth = 1.0f;//大于1需要开启gpu特性
		mPipeline->mRasterState.cullMode = VK_CULL_MODE_BACK_BIT;
		mPipeline->mRasterState.frontFace = VK_FRONT_FACE_CLOCKWISE;

		mPipeline->mRasterState.depthBiasEnable = VK_FALSE;
		mPipeline->mRasterState.depthBiasConstantFactor = 0.0f;
		mPipeline->mRasterState.depthBiasClamp = 0.0f;
		mPipeline->mRasterState.depthBiasSlopeFactor = 0.0f;

		//TODO:多重采样
		mPipeline->mSampleState.sampleShadingEnable = VK_FALSE;
		mPipeline->mSampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		mPipeline->mSampleState.minSampleShading = 1.0f;
		mPipeline->mSampleState.pSampleMask = nullptr;
		mPipeline->mSampleState.alphaToCoverageEnable = VK_FALSE;
		mPipeline->mSampleState.alphaToOneEnable = VK_FALSE;

		//TODO:深度与模板测试

		//颜色混合

		//这个是颜色混合掩码，得到的混合结果，按照通道与掩码进行AND操作，输出
		VkPipelineColorBlendAttachmentState blendAttachment{};
		blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;

		blendAttachment.blendEnable = VK_FALSE;
		blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;

		blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		mPipeline->pushBlendAttachment(blendAttachment);

		//1 blend有两种计算方式，第一种如上所述，进行alpha为基础的计算，第二种进行位运算
		//2 如果开启了logicOp，那么上方设置的alpha为基础的运算，失灵
		//3 ColorWrite掩码，仍然有效，即便开启了logicOP
		//4 因为，我们可能会有多个FrameBuffer输出，所以可能需要多个BlendAttachment
		mPipeline->mBlendState.logicOpEnable = VK_FALSE;
		mPipeline->mBlendState.logicOp = VK_LOGIC_OP_COPY;

		//配合blendAttachment的factor与operation
		mPipeline->mBlendState.blendConstants[0] = 0.0f;
		mPipeline->mBlendState.blendConstants[1] = 0.0f;
		mPipeline->mBlendState.blendConstants[2] = 0.0f;
		mPipeline->mBlendState.blendConstants[3] = 0.0f;

		//uniform的传递
		mPipeline->mLayoutState.setLayoutCount = 0;
		mPipeline->mLayoutState.pSetLayouts = nullptr;
		mPipeline->mLayoutState.pushConstantRangeCount = 0;
		mPipeline->mLayoutState.pPushConstantRanges = nullptr;

		mPipeline->build();
	}

	void Application::createRenderPass() {
		//输入画布的描述
		VkAttachmentDescription attachmentDes{};
        //画布的颜色格式
		attachmentDes.format = mSwapChain->getFormat();
        //不需要多重采样, 设置1
		attachmentDes.samples = VK_SAMPLE_COUNT_1_BIT;
        //渲染之前清理画布
		attachmentDes.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        //渲染之后存储内容
		attachmentDes.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        //不关心模板数据
		attachmentDes.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDes.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        //渲染过程开始之前图像将具有的布局, VK_IMAGE_LAYOUT_UNDEFINED不能保证图像的内容被保留, 但这并不重要，因为我们自己也需要要清除画布
		attachmentDes.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //渲染过程完成时自动转换到的布局, 在渲染后使用交换链准备好呈现，这就是我们使用VK_IMAGE_LAYOUT_PRESENT_SRC_KHR。
		attachmentDes.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		mRenderPass->addAttachment(attachmentDes);

		//对于画布的索引设置以及格式要求
		VkAttachmentReference attachmentRef{};
		attachmentRef.attachment = 0;
		attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		//创建子通道
		Wrapper::SubPass subPass{};
		subPass.addColorAttachmentReference(attachmentRef);
		subPass.buildSubPassDescription();
		
        mRenderPass->addSubPass(subPass);

		//子流程之间的依赖关系
		VkSubpassDependency dependency{};
        //特殊值VK_SUBPASS_EXTERNAL是指渲染通道之前或之后的隐式子通道，具体取决于它是否在 srcSubpass或中指定dstSubpass
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        //索引0指的是我们的子通道，它是第一个也是唯一一个, dstSubpass必须始终高于以防止依赖图中的srcSubpass循环（除非子通道之一是 VK_SUBPASS_EXTERNAL）
		dependency.dstSubpass = 0;
        
        //这两个字段指定要等待的操作以及这些操作发生的阶段。我们需要等待交换链完成对图像的读取，然后才能访问它。这可以通过等待颜色附件输出级本身来完成。
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		mRenderPass->addDependency(dependency);

		mRenderPass->buildRenderPass();
	}


	void Application::mainLoop() {
		while (!mWindow->shouldClose()) {
			mWindow->pollEvents();
		}
	}

	void Application::cleanUp() {

		mPipeline.reset();

		mRenderPass.reset();

		mSwapChain.reset();

		mDevice.reset();
		mSurface.reset();
		mInstance.reset();
		mWindow.reset();
	}
}
