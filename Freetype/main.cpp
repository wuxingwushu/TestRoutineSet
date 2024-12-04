#include "../src/Vulkan/buffer.h"
#include "../src/Vulkan/SwapChain.h"
#include "../src/ImGui/imgui_impl_vulkan.h"
#include "../src/ImGui/Interface.h"
#include "../src/VulkanTool/PipelineS.h"
#include "../src/Vulkan/semaphore.h"
#include "../src/Vulkan/fence.h"
#include "../src/VulkanTool/PixelTexture.h"
#include "../src/Vulkan/descriptorSet.h"
#include "../src/Tool/FastNoiseLite.h"
#include "../src/Tool/PerlinNoise.h"
#include <string>

#include <freetype/freetype.h>
#include FT_FREETYPE_H

VulKan::Window* mWindow{ nullptr };//创建窗口
VulKan::Instance* mInstance{ nullptr };//实列化需要的VulKan功能APi
VulKan::Device* mDevice{ nullptr };//获取电脑的硬件设备
VulKan::CommandPool* mCommandPool{ nullptr };//设置渲染指令的工作
VulKan::WindowSurface* mWindowSurface{ nullptr };//获取你在什么平台运行调用不同的API（比如：Window，Android）
VulKan::SwapChain* mSwapChain{ nullptr };//设置VulKan的工作细节
VulKan::RenderPass* mRenderPass{ nullptr };//设置GPU画布
VulKan::PipelineS* mPipelineS{ nullptr }; //渲染管线集合
VulKan::CommandBuffer* mImGuuiCommandBuffers{ nullptr };
VulKan::Sampler* mSampler{ nullptr };
VulKan::PixelTexture* mTexture{ nullptr };

std::vector<VulKan::Semaphore*> mImageAvailableSemaphores{};//图片显示完毕信号量
std::vector<VulKan::Semaphore*> mRenderFinishedSemaphores{};//图片渲染完成信号量
std::vector<VulKan::Fence*> mFences{};//控制管线工作，比如（下一个管线需要上一个管线的图片，那就等上一个管线图片输入进来才开始工作）

int mCurrentFrame{ 0 };//当前是渲染哪一张GPU画布



//ImGui 错误信息回调函数
static void check_vk_result(VkResult err)
{
	if (err == 0)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}

void createRenderPass(VulKan::RenderPass* wRenderPass, VulKan::SwapChain* wSwapChain, VulKan::Device* wDevice) {
	//0：最终输出图片 1：Resolve图片（MutiSample） 2：Depth图片

	//0号位：是SwapChain原来那张图片，是Resolve的目标点，即需要设置到SubPass的Resolve当中
	VkAttachmentDescription finalAttachmentDes{};
	finalAttachmentDes.format = wSwapChain->getFormat();//获取图片格式
	finalAttachmentDes.samples = VK_SAMPLE_COUNT_1_BIT;//采样
	finalAttachmentDes.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//原来上一张的图片，应该怎么处理（这里是不管直接清空）
	finalAttachmentDes.storeOp = VK_ATTACHMENT_STORE_OP_STORE;//输出图片怎么处理（这里是保存）
	finalAttachmentDes.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;//我们没有用到他，所以不关心
	finalAttachmentDes.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//我们没有用到他，所以不关心
	finalAttachmentDes.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//
	finalAttachmentDes.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;//保存的格式，是一直适合显示的格式

	wRenderPass->addAttachment(finalAttachmentDes);

	//1号位：被Resolve的图片，即多重采样的源头图片，也即颜色输出的目标图片
	VkAttachmentDescription MutiAttachmentDes{};
	MutiAttachmentDes.format = wSwapChain->getFormat();
	MutiAttachmentDes.samples = wDevice->getMaxUsableSampleCount();
	MutiAttachmentDes.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	MutiAttachmentDes.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	MutiAttachmentDes.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	MutiAttachmentDes.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	MutiAttachmentDes.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	MutiAttachmentDes.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	wRenderPass->addAttachment(MutiAttachmentDes);


	//2号位：深度缓存attachment
	VkAttachmentDescription depthAttachmentDes{};
	depthAttachmentDes.format = VulKan::Image::findDepthFormat(wDevice);
	depthAttachmentDes.samples = wDevice->getMaxUsableSampleCount();
	depthAttachmentDes.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachmentDes.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachmentDes.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachmentDes.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachmentDes.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachmentDes.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	wRenderPass->addAttachment(depthAttachmentDes);

	//对于画布的索引设置以及格式要求
	VkAttachmentReference finalAttachmentRef{};
	finalAttachmentRef.attachment = 0;//画布的索引
	finalAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;//我们希望的格式

	VkAttachmentReference mutiAttachmentRef{};
	mutiAttachmentRef.attachment = 1;
	mutiAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 2;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//创建子流程
	VulKan::SubPass subPass{};
	subPass.addColorAttachmentReference(mutiAttachmentRef);
	subPass.setDepthStencilAttachmentReference(depthAttachmentRef);
	subPass.setResolveAttachmentReference(finalAttachmentRef);

	subPass.buildSubPassDescription();

	wRenderPass->addSubPass(subPass);

	//子流程之间的依赖关系
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;//vulkan 给的开始的虚拟流程的
	dependency.dstSubpass = 0;//代表的是 mRenderPass 数组的第 0 个 SubPass
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;//我上一个流程，进行到那个一步可以进行下一个子流程
	dependency.srcAccessMask = 0;//上一个流程到那一步算结束（0 代表我不关心，下一个流程直接开始）
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;//下一个流程要在哪里等待上一个流程结束
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;//我到了 图片读写操作是要等待上一个子流程结束

	wRenderPass->addDependency(dependency);

	wRenderPass->buildRenderPass();
}

void recreateSwapChain() {
	int width = 0, height = 0;
	glfwGetFramebufferSize(mWindow->getWindow(), &width, &height);
	while (width == 0 || height == 0) {
		glfwWaitEvents();
		glfwGetFramebufferSize(mWindow->getWindow(), &width, &height);
	}
	vkDeviceWaitIdle(mDevice->getDevice());

	mSwapChain->~SwapChain();
	mSwapChain->StructureSwapChain();
	Global::mWidth = mSwapChain->getExtent().width;
	Global::mHeight = mSwapChain->getExtent().height;
	mRenderPass->~RenderPass();
	createRenderPass(mRenderPass, mSwapChain, mDevice);
	mSwapChain->createFrameBuffers(mRenderPass);
	mPipelineS->ReconfigurationPipelineS();
}

void Render() {
	//等待当前要提交的CommandBuffer执行完毕
	mFences[mCurrentFrame]->block();


	//获取交换链当中的下一帧
	uint32_t imageIndex{ 0 };
	VkResult result = vkAcquireNextImageKHR(
		mDevice->getDevice(),
		mSwapChain->getSwapChain(),
		UINT64_MAX,
		mImageAvailableSemaphores[mCurrentFrame]->getSemaphore(),
		VK_NULL_HANDLE,
		&imageIndex);

	//窗体发生了尺寸变化 （窗口大小改变了话，指令要全部重新录制）
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		mWindow->mWindowResized = false;
	}//VK_SUBOPTIMAL_KHR得到了一张认为可用的图像，但是表面格式不一定匹配
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Error: failed to acquire next image");
	}


	//构建提交信息
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;


	//同步信息，渲染对于显示图像的依赖，显示完毕后，才能输出颜色
	VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame]->getSemaphore() };//获取储存信号量类
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };//管线颜色输出阶段，等待信号量

	submitInfo.waitSemaphoreCount = 1;//等待多少个信号量
	submitInfo.pWaitSemaphores = waitSemaphores;//储存到那个信号量
	submitInfo.pWaitDstStageMask = waitStages;//那个阶段等待信号量


	VkCommandBuffer commandBuffers[1];
	//指定提交哪些命令
	commandBuffers[0] = { mImGuuiCommandBuffers->getCommandBuffer() };
	submitInfo.commandBufferCount = (uint32_t)1;
	submitInfo.pCommandBuffers = commandBuffers;

	VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame]->getSemaphore() };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	mFences[mCurrentFrame]->resetFence();
	if (vkQueueSubmit(mDevice->getGraphicQueue(), 1, &submitInfo, mFences[mCurrentFrame]->getFence()) != VK_SUCCESS) {
		throw std::runtime_error("Error:failed to submit renderCommand");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { mSwapChain->getSwapChain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	//如果开了帧数限制，GPU会在这里等待，让当前循环符合GPU设置的帧数
	result = vkQueuePresentKHR(mDevice->getPresentQueue(), &presentInfo);//更新画面

	//由于驱动程序不一定精准，所以我们还需要用自己的标志位判断
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mWindow->mWindowResized) {
		recreateSwapChain();
		mWindow->mWindowResized = false;
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed to present");
	}

	mCurrentFrame = (mCurrentFrame + 1) % mSwapChain->getImageCount();
}

FT_Library library;
FT_Face face;

void UpdateAtlas() {
	unsigned char* P = (unsigned char*)mTexture->getHOSTImagePointer();

	unsigned char* CpyP;
	unsigned char color;
	unsigned int Width = 0;
	// 渲染字体
	FT_UInt glyph_index;
	FT_GlyphSlot slot = face->glyph;

	wchar_t* chineseText = L"素净";
	for (int i = 0; i < wcslen(chineseText); ++i) {
		FT_UInt glyph_index = FT_Get_Char_Index(face, chineseText[i]);
		if (FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT)) {
			Width += (16 * 4);
			continue;
		}
		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		// 在这里处理渲染后的字符位图，将其显示在屏幕上或者保存为图片等操作
		CpyP = P;
		CpyP += Width;//左右平移
		CpyP += (1920 * 4 * (59 - slot->bitmap_top));//上下平移
		//std::cout << slot->bitmap_left << " - " << slot->bitmap_top << std::endl;
		//std::cout << slot->bitmap.rows << " - " << slot->bitmap.width << std::endl;
		for (size_t ix = 0; ix < 1080; ix++)
		{
			if (slot->bitmap.rows <= ix) {
				//P += ((1079 - ix) * 4);
				break;
			}
			for (int iy = 0; iy < 1920; iy++)
			{
				if (slot->bitmap.width <= iy) {
					CpyP += ((1920 - iy) * 4);
					break;
				}
				if (slot->bitmap.buffer[ix * slot->bitmap.width + iy])
					color = 255;
				else
					color = 0;

				*CpyP = color;
				++CpyP;
				*CpyP = color;
				++CpyP;
				*CpyP = color;
				++CpyP;
				*CpyP = color;
				++CpyP;
			}
		}
		Width += (slot->bitmap.width * 4 + 16);
	}
	
	mTexture->endHOSTImagePointer();
	mTexture->UpDataImage();
}

int main() {
	/************************** 初始化窗口和VulKan **************************/
	mWindow = new VulKan::Window(1920, 1080, false, false);
	mInstance = new VulKan::Instance(false);
	mWindowSurface = new VulKan::WindowSurface(mInstance, mWindow);//获取你在什么平台运行调用不同的API（比如：Window，Android）
	mDevice = new VulKan::Device(mInstance, mWindowSurface);//获取电脑的硬件设备，vma内存分配器 也在这里被创建了
	mCommandPool = new VulKan::CommandPool(mDevice);//创建指令池，给CommandBuffer用的
	mSwapChain = new VulKan::SwapChain(mDevice, mWindow, mWindowSurface, mCommandPool);//设置VulKan的工作细节
	mRenderPass = new VulKan::RenderPass(mDevice);//创建GPU画布描述
	createRenderPass(mRenderPass, mSwapChain, mDevice);
	mSwapChain->createFrameBuffers(mRenderPass);//把GPU画布描述传给交换链生成GPU画布
	Global::mWidth = mSwapChain->getExtent().width;
	Global::mHeight = mSwapChain->getExtent().height;
	mSampler = new VulKan::Sampler(mDevice);

	for (int i = 0; i < mSwapChain->getImageCount(); ++i) {
		VulKan::Semaphore* imageSemaphore = new VulKan::Semaphore(mDevice);
		mImageAvailableSemaphores.push_back(imageSemaphore);

		VulKan::Semaphore* renderSemaphore = new VulKan::Semaphore(mDevice);
		mRenderFinishedSemaphores.push_back(renderSemaphore);

		VulKan::Fence* fence = new VulKan::Fence(mDevice);
		mFences.push_back(fence);
	}
	/************************** 初始化ImGui **************************/
	//准备填写需要的信息
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = mInstance->getInstance();
	init_info.PhysicalDevice = mDevice->getPhysicalDevice();
	init_info.Device = mDevice->getDevice();
	init_info.QueueFamily = mDevice->getGraphicQueueFamily().value();
	init_info.Queue = mDevice->getGraphicQueue();
	init_info.PipelineCache = nullptr;
	init_info.Subpass = 0;
	init_info.ImageCount = mSwapChain->getImageCount();
	init_info.MSAASamples = mDevice->getMaxUsableSampleCount();
	init_info.Allocator = nullptr;//内存分配器
	init_info.CheckVkResultFn = check_vk_result;//错误处理

	mImGuuiCommandBuffers = new VulKan::CommandBuffer(mDevice, mCommandPool);

	GAME::ImGuiInterFace* InterFace = new GAME::ImGuiInterFace(mDevice, mWindow, init_info, mRenderPass, mImGuuiCommandBuffers, mSwapChain->getImageCount());

	VkRenderPassBeginInfo renderBeginInfo{};
	renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderBeginInfo.renderArea.offset = { 0, 0 };//画布从哪里开始画
	

	//0：final   1：muti   2：depth
	std::vector< VkClearValue> clearColors{};//在使用画布前，用什么颜色清理他，
	VkClearValue finalClearColor{};
	finalClearColor.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	clearColors.push_back(finalClearColor);

	VkClearValue mutiClearColor{};
	mutiClearColor.color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearColors.push_back(mutiClearColor);

	VkClearValue depthClearColor{};
	depthClearColor.depthStencil = { 1.0f, 0 };
	clearColors.push_back(depthClearColor);

	renderBeginInfo.clearValueCount = static_cast<uint32_t>(clearColors.size());//有多少个
	renderBeginInfo.pClearValues = clearColors.data();//用来清理的颜色数据

	mTexture = new VulKan::PixelTexture(mDevice, mCommandPool, nullptr, 1920, 1080, 4, mSampler);

	/******************** 字体加载 ********************/

	if (FT_Init_FreeType(&library)) {
		std::cerr << "Failed to initialize FreeType library" << std::endl;
		return -1;
	}

	if (FT_New_Face(library, "./Minecraft_AE.ttf", 0, &face)) {
		std::cerr << "Failed to load font" << std::endl;
		FT_Done_FreeType(library);
		return -1;
	}

	if (FT_Set_Char_Size(face, 0, 32 * 72, 72, 72)) {
	//if (FT_Set_Pixel_Sizes(face, 32, 32)) {
		std::cerr << "Failed to set character size" << std::endl;
		FT_Done_Face(face);
		FT_Done_FreeType(library);
		return -1;
	}

	/**************************************************/

	UpdateAtlas();

	std::vector<VulKan::UniformParameter*> mUniformParameter;
	VulKan::UniformParameter* textureParam = new VulKan::UniformParameter();
	textureParam->mBinding = 0;
	textureParam->mCount = 1;
	textureParam->mDescriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureParam->mStage = VK_SHADER_STAGE_FRAGMENT_BIT;
	textureParam->mPixelTexture = mTexture;
	mUniformParameter.push_back(textureParam);

	VulKan::DescriptorPool* mDescriptorPool = new VulKan::DescriptorPool(mDevice);
	mDescriptorPool->build(mUniformParameter, mSwapChain->getImageCount());

	VkDescriptorSetLayout mVkDescriptorSetLayout;

	VkDescriptorSetLayoutBinding binding[1] = {};
	binding[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	binding[0].descriptorCount = 1;
	binding[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	VkDescriptorSetLayoutCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info.bindingCount = 1;
	info.pBindings = binding;
	vkCreateDescriptorSetLayout(mDevice->getDevice(), &info, nullptr, &mVkDescriptorSetLayout);

	VulKan::DescriptorSet* mDescriptorSet = new VulKan::DescriptorSet(mDevice, mUniformParameter, 
		mVkDescriptorSetLayout, mDescriptorPool, mSwapChain->getImageCount());

	while (!mWindow->shouldClose())
	{
		mWindow->pollEvents();

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin(u8"控制台 ");
		ImGui::SetWindowPos(ImVec2{ 0,0 });
		ImGui::SetWindowSize(ImVec2{ float(Global::mWidth * 0.2f), float(Global::mHeight) });
		if (ImGui::Button(u8"渲染")) {
			UpdateAtlas();
		}
		ImGui::End();
		ImGui::Begin(u8"监视器 ");
		ImGui::SetWindowPos(ImVec2{ Global::mWidth * 0.2f,0 });
		ImGui::SetWindowSize(ImVec2{ float(Global::mWidth * 0.8f), float(Global::mHeight) });
		ImGui::Image((ImTextureID)mDescriptorSet->getDescriptorSet(mCurrentFrame), ImVec2{ float(Global::mWidth * 0.8f), float(Global::mHeight * 0.8f) });
		ImGui::End();
		ImGui::Render();

		renderBeginInfo.framebuffer = mSwapChain->getFrameBuffer(mCurrentFrame);//设置是那个GPU画布
		renderBeginInfo.renderPass = mRenderPass->getRenderPass();//获得画布信息
		renderBeginInfo.renderArea.extent = mSwapChain->getExtent();//画到哪里结束
		mImGuuiCommandBuffers->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		mImGuuiCommandBuffers->beginRenderPass(renderBeginInfo);
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), mImGuuiCommandBuffers->getCommandBuffer());
		mImGuuiCommandBuffers->endRenderPass();
		mImGuuiCommandBuffers->end();
		Render();
	}

	//关闭字体
	FT_Done_Face(face);
	FT_Done_FreeType(library);

	return 0;
}
