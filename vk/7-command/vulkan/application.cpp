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

        //创建命令缓冲池
		mCommandPool = Wrapper::CommandPool::create(mDevice);

		mCommandBuffers.resize(mSwapChain->getImageCount());

        //从命令缓冲池里创建命令缓冲
		for (int i = 0; i < mSwapChain->getImageCount(); ++i) {
			mCommandBuffers[i] = Wrapper::CommandBuffer::create(mDevice, mCommandPool);

			mCommandBuffers[i]->begin();

			VkRenderPassBeginInfo renderBeginInfo{};
			renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderBeginInfo.renderPass = mRenderPass->getRenderPass(); //指定使用哪个renderpass渲染
			renderBeginInfo.framebuffer = mSwapChain->getFrameBuffer(i);//指定用哪个fbo
			renderBeginInfo.renderArea.offset = {0, 0};//渲染的起始点
			renderBeginInfo.renderArea.extent = mSwapChain->getExtent();//渲染的宽高

            VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 0.0f }; //清屏颜色和深度值
			renderBeginInfo.clearValueCount = 1;
			renderBeginInfo.pClearValues = &clearColor;

			mCommandBuffers[i]->beginRenderPass(renderBeginInfo);

            //把渲染管道和渲染命令绑定
			mCommandBuffers[i]->bindGraphicPipeline(mPipeline->getPipeline());

			mCommandBuffers[i]->draw(3);

			mCommandBuffers[i]->endRenderPass();

			mCommandBuffers[i]->end();
		}

		for (int i = 0; i < mSwapChain->getImageCount(); ++i) {
			auto imageSemaphore = Wrapper::Semaphore::create(mDevice);
			mImageAvailableSemaphores.push_back(imageSemaphore);

			auto renderSemaphore = Wrapper::Semaphore::create(mDevice);
			mRenderFinishedSemaphores.push_back(renderSemaphore);

			auto fence = Wrapper::Fence::create(mDevice);
			mFences.push_back(fence);
		}
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

		auto shaderVertex = Wrapper::Shader::create(mDevice, "/Users/jason/Jason/project/vulkan-tutorial/vk/7-command/vulkan/shaders/vs.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
		shaderGroup.push_back(shaderVertex);

		auto shaderFragment = Wrapper::Shader::create(mDevice, "/Users/jason/Jason/project/vulkan-tutorial/vk/7-command/vulkan/shaders/fs.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");
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
		attachmentDes.format = mSwapChain->getFormat();
		attachmentDes.samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDes.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDes.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDes.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDes.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDes.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDes.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		mRenderPass->addAttachment(attachmentDes);

		//对于画布的索引设置以及格式要求
		VkAttachmentReference attachmentRef{};
		attachmentRef.attachment = 0;
		attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		//创建子流程
		Wrapper::SubPass subPass{};
		subPass.addColorAttachmentReference(attachmentRef);
		subPass.buildSubPassDescription();

		mRenderPass->addSubPass(subPass);

		//子流程之间的依赖关系
		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
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

			render();
		}

		vkDeviceWaitIdle(mDevice->getDevice());
	}

	void Application::render() {
		//等待当前要提交的CommandBuffer执行完毕
		mFences[mCurrentFrame]->block();

		//获取交换链当中的下一帧
		uint32_t imageIndex{ 0 };
		vkAcquireNextImageKHR(mDevice->getDevice(), //与swapChain关联的device
                              mSwapChain->getSwapChain(),//要从哪个swapChain获取image
                              UINT64_MAX,//如果没有可用的图像，指定函数等待的时间（以纳秒为单位）
                              mImageAvailableSemaphores[mCurrentFrame]->getSemaphore(), //为VK_NULL_HANDLE或者使用信号量
                              VK_NULL_HANDLE, //为VK_NULL_HANDLE或者使用fence
                              &imageIndex); //返回可使用的image索引

		//构建提交信息
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		

		//同步信息，渲染对于显示图像的依赖，显示完毕后，才能输出颜色
		VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame]->getSemaphore() };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		//指定提交哪些命令
		auto commandBuffer = mCommandBuffers[imageIndex]->getCommandBuffer();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame]->getSemaphore()};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		mFences[mCurrentFrame]->resetFence();
        //在vkQueueSubmit/vkQueueBindSparse的时候，可以附加带上一个Fence对象。之后就可以使用这个对象来查询之前操作是否完成
		if (vkQueueSubmit(mDevice->getGraphicQueue(), 1, &submitInfo, mFences[mCurrentFrame]->getFence()) != VK_SUCCESS) {
			throw std::runtime_error("Error:failed to submit renderCommand");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1; //发出当前请求之前要等待的信号量数。该数字可能为零。
		presentInfo.pWaitSemaphores = signalSemaphores;//发出当前请求之前要等待的信号量。

		VkSwapchainKHR swapChains[] = {mSwapChain->getSwapChain()};
		presentInfo.swapchainCount = 1;//提供给的交换链的数量
		presentInfo.pSwapchains = swapChains;//交换链数组指针

        //swapChain中的第几帧
		presentInfo.pImageIndices = &imageIndex;

        //在对所有渲染命令进行排队并将图像转换为正确的布局后，将图像排队以进行显示
		vkQueuePresentKHR(mDevice->getPresentQueue(), &presentInfo);

		mCurrentFrame = (mCurrentFrame + 1) % mSwapChain->getImageCount();
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
