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
#include "../src/VulkanTool/stb_image.h"
#include "../src/Tool/Timer.h"
#include "../src/Tool/Tool.h"


Timer mTimer(10);


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


int TextureW = 5000, TextureH = 5000;


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

//转换为灰度图
void ConversionGrayscaleImage(unsigned char* outimg, unsigned char* img, unsigned int width, unsigned int height, unsigned int channels) {
	for (int i = 0; i < (width * height); ++i) {
		// 根据 RGB 分量计算灰度值
		outimg[i] = (unsigned char)(0.2126f * img[channels * i] + 0.7152f * img[channels * i + 1] + 0.0722f * img[channels * i + 2]);
	}
}

//模糊
void myBlur(const unsigned char* src, unsigned char* dst, int W, int H, int kernelSize) {
	int kernelRadius = kernelSize / 2;

	// 遍历图像中的每个像素
	for (int y = 0; y < H; ++y) {
		for (int x = 0; x < W; ++x) {
			int sum = 0;

			// 计算模糊区域内像素的和
			for (int ky = -kernelRadius; ky <= kernelRadius; ++ky) {
				for (int kx = -kernelRadius; kx <= kernelRadius; ++kx) {
					int nx = x + kx;
					int ny = y + ky;

					// 边界检查，超出图像范围的像素使用原始像素值
					nx = std::max(0, std::min(W - 1, nx));
					ny = std::max(0, std::min(H - 1, ny));

					sum += src[ny * W + nx];
				}
			}

			// 将均值存储到输出图像中
			dst[y * W + x] = sum / (kernelSize * kernelSize);
		}
	}
}




unsigned char* P = nullptr;
// 模拟 GetPix 函数，打印每个点的像素坐标
void GetPix(unsigned int y, unsigned int x, float D = 1.0f)
{
	if (y >= TextureW)return;
	if (x >= TextureH)return;
	//std::cout << "x: " << x << ", y: " << y << std::endl;
	P[(x * TextureW + y) * 4 + 0] = 255 * D;
	P[(x * TextureW + y) * 4 + 1] = 255 * D;
	P[(x * TextureW + y) * 4 + 2] = 255 * D;
	P[(x * TextureW + y) * 4 + 3] = 255 * D;
}
void GetPix2(unsigned int y, unsigned int x, float D = 1.0f)
{
	if (y >= TextureW)return;
	if (x >= TextureH)return;
	//std::cout << "x: " << x << ", y: " << y << std::endl;
	P[(x * TextureW + y) * 4 + 0] = 255 * D;
	P[(x * TextureW + y) * 4 + 1] = 255 * D;
	P[(x * TextureW + y) * 4 + 2] = 255 * D;
	P[(x * TextureW + y) * 4 + 3] = 255;
}

void GetPix(unsigned int y, unsigned int x, unsigned char* color)
{
	if (y >= TextureW)return;
	if (x >= TextureH)return;
	//std::cout << "x: " << x << ", y: " << y << std::endl;
	P[(x * TextureW + y) * 4 + 0] = color[0];
	P[(x * TextureW + y) * 4 + 1] = color[1];
	P[(x * TextureW + y) * 4 + 2] = color[2];
	P[(x * TextureW + y) * 4 + 3] = 255;
}

void Bresenham(int x1, int y1, int x2, int y2) {
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	int sx = (x1 < x2) ? 1 : -1;
	int sy = (y1 < y2) ? 1 : -1;
	int err = dx - dy;
	unsigned char clr[3] = { 0,0,255 };
	GetPix(x1, y1, clr);
	while (true) {
		if (x1 == x2 && y1 == y2) {
			break;
		}

		int e2 = 2 * err;

		if (e2 > -dy) {
			err -= dy;
			x1 += sx;
			GetPix(x1, y1, clr);
		}

		if (e2 < dx) {
			err += dx;
			y1 += sy;
			GetPix(x1, y1, clr);
		}
	}
}

void Bresenham2(int x1, int y1, int x2, int y2) {
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	int sx = (x1 < x2) ? 1 : -1;
	int sy = (y1 < y2) ? 1 : -1;
	int err = dx - dy;
	unsigned char clr[3] = { 255,0,0 };
	GetPix(x1, y1, clr);
	while (true) {
		if (x1 == x2 && y1 == y2) {
			break;
		}

		int e2 = 2 * err;

		if (e2 > -dy) {
			err -= dy;
			x1 += sx;
			GetPix(x1, y1, clr);
		}

		if (e2 < dx) {
			err += dx;
			y1 += sy;
			GetPix(x1, y1, clr);
		}
	}
}



bool isRectangle(std::vector < std::pair<int, int>>& Spot) {
	//坐标点按照顺时针排列
	std::pair<float, float> Core = { Spot[0].first + Spot[1].first + Spot[2].first + Spot[3].first, Spot[0].second + Spot[1].second + Spot[2].second + Spot[3].second };
	Core.first /= 4; Core.second /= 4;

	std::vector<std::pair<float, float>> Angle;
	Angle.push_back({ Spot[0].first - Core.first, Spot[0].second - Core.second });
	Angle.push_back({ Spot[1].first - Core.first, Spot[1].second - Core.second });
	Angle.push_back({ Spot[2].first - Core.first, Spot[2].second - Core.second });
	Angle.push_back({ Spot[3].first - Core.first, Spot[3].second - Core.second });

	for (size_t i = 0; i < Angle.size(); i++)
	{
		Angle[i].first = atan2(Angle[i].first, Angle[i].second);
	}
	int min = 0, max = 0;
	for (size_t i = 0; i < Angle.size() - 1; i++)
	{
		if (Angle[min].first > Angle[i + 1].first) {
			min = i + 1;
		}
		if (Angle[max].first < Angle[i + 1].first) {
			max = i + 1;
		}
	}
	if (min != 0) {
		std::swap(Angle[min].first, Angle[0].first);
		std::swap(Spot[min], Spot[0]);
	}
	if (max != (Angle.size() - 1)) {
		std::swap(Angle[max].first, Angle[Angle.size() - 1].first);
		std::swap(Spot[max], Spot[Angle.size() - 1]);
	}
	if (Angle[1].first > Angle[2].first) {
		std::swap(Spot[1], Spot[2]);
	}

	// 判断四个点是否构成矩形
	double d1 = sqrt((Spot[0].first - Spot[1].first) * (Spot[0].first - Spot[1].first) + (Spot[0].second - Spot[1].second) * (Spot[0].second - Spot[1].second));
	double d3 = sqrt((Spot[2].first - Spot[3].first) * (Spot[2].first - Spot[3].first) + (Spot[2].second - Spot[3].second) * (Spot[2].second - Spot[3].second));
	d1 -= d3;
	if (fabs(d1) > 15) return false;

	double d2 = sqrt((Spot[1].first - Spot[2].first) * (Spot[1].first - Spot[2].first) + (Spot[1].second - Spot[2].second) * (Spot[1].second - Spot[2].second));
	double d4 = sqrt((Spot[3].first - Spot[0].first) * (Spot[3].first - Spot[0].first) + (Spot[3].second - Spot[0].second) * (Spot[3].second - Spot[0].second));
	d2 -= d4;
	if (fabs(d2) > 15) return false;

	double diagonal1 = sqrt((Spot[0].first - Spot[2].first) * (Spot[0].first - Spot[2].first) + (Spot[0].second - Spot[2].second) * (Spot[0].second - Spot[2].second));
	double diagonal2 = sqrt((Spot[1].first - Spot[3].first) * (Spot[1].first - Spot[3].first) + (Spot[1].second - Spot[3].second) * (Spot[1].second - Spot[3].second));
	diagonal1 -= diagonal2;
	return (fabs(diagonal1) < 15);
}

std::pair<int, int> findIntersection(const std::pair<int, int>& p1, const std::pair<int, int>& p2, const std::pair<int, int>& p3, const std::pair<int, int>& p4) {
	// 计算两条直线的交点
	double a1 = p2.second - p1.second;
	double b1 = p1.first - p2.first;
	double c1 = a1 * p1.first + b1 * p1.second;

	double a2 = p4.second - p3.second;
	double b2 = p3.first - p4.first;
	double c2 = a2 * p3.first + b2 * p3.second;

	double det = a1 * b2 - a2 * b1;
	if (det == 0) {
		// 平行线没有交点
		return { 0, 0 };
	}
	else {
		double x = (b2 * c1 - b1 * c2) / det;
		double y = (a1 * c2 - a2 * c1) / det;
		return { x, y };
	}
}

//判断该点是否成立
bool dian_Establish(unsigned char* img, unsigned int h, unsigned int w, std::pair<int, int> Dian, int Bchang) {
	bool Collision = false;
	int x1, x2, y1, y2;
	x1 = (Dian.first - Bchang);
	y1 = (Dian.second - Bchang);
	if (x1 < 0) x1 = 0;
	if (y1 < 0) y1 = 0;
	x2 = (Dian.first + Bchang);
	y2 = (Dian.second + Bchang);
	if (x2 > w) x2 = w;
	if (y2 > h) x2 = h;
	for (int x = x1; x < x2; ++x)
	{
		for (int y = y1; y < y2; ++y)
		{
			if (img[(y * w) + x] > 150) {
				Collision = true;
				return Collision;
			}
		}
	}
	return Collision;
}

std::vector<unsigned char> BoxImg(unsigned char* Data, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int Yw) {
	std::vector<unsigned char> Img(w * h);
	w = x + w;
	h = y + h;
	int s = 0;
	for (size_t j = y; j < h; j++)
	{
		for (size_t i = x; i < w; i++)
		{
			Img[s] = Data[(j * Yw) + i];
			++s;
		}
	}
	return Img;
}

void Dithering(unsigned char* Data, unsigned int w, unsigned int h) {

	int pixX = TextureW - w;
	int pixY = TextureH - h;
	// 进行抖动处理
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			// 根据阈值进行二值化
			unsigned char threshold = (Data[y * w + x] > 128) ? 255 : 0;

			// 计算误差
			int error = static_cast<int>(Data[y * w + x]) - threshold;

			// 将误差传播到相邻像素
			if (x < w - 1) {
				if (Data[y * w + x + 1] + error * 7 / 16 > 255) {
					Data[y * w + x + 1] = 255;
				}
				else if (Data[y * w + x + 1] + error * 7 / 16 < 0) {
					Data[y * w + x + 1] = 0;
				}
				else {
					Data[y * w + x + 1] += error * 7 / 16;
				}
			}
			if (y < h - 1) {
				if (x > 0) {
					if (Data[(y + 1) * w + x - 1] + error * 3 / 16 > 255) {
						Data[(y + 1) * w + x - 1] = 255;
					}
					else if (Data[(y + 1) * w + x - 1] + error * 3 / 16 < 0) {
						Data[(y + 1) * w + x - 1] = 0;
					}
					else {
						Data[(y + 1) * w + x - 1] += error * 3 / 16;
					}
				}
				if (Data[(y + 1) * w + x] + error * 5 / 16 > 255) {
					Data[(y + 1) * w + x] = 255;
				}
				else if (Data[(y + 1) * w + x] + error * 5 / 16 < 0) {
					Data[(y + 1) * w + x] = 0;
				}
				else {
					Data[(y + 1) * w + x] += error * 5 / 16;
				}
				if (x < w - 1) {
					if (Data[(y + 1) * w + x + 1] + error * 1 / 16 > 255) {
						Data[(y + 1) * w + x + 1] = 255;
					}
					else if (Data[(y + 1) * w + x + 1] + error * 1 / 16 < 0) {
						Data[(y + 1) * w + x + 1] = 0;
					}
					else {
						Data[(y + 1) * w + x + 1] += error * 1 / 16;
					}
				}
			}

			// 设置像素值
			Data[y * w + x] = threshold;
			GetPix2(pixX + x, pixY + y, float(threshold) / 255);
		}
	}
}




const char Info_1[] = R"(const unsigned char ImgD[][3813] = {)";
const char Info_2[] = R"(
};)";
std::string IMGSTR{};
void ImgData(unsigned char* Data, unsigned int w, unsigned int h) {
	unsigned char S = 0;
	unsigned char N = 0;
	unsigned char color = 0;
	IMGSTR += "\n{\n\t";
	for (size_t i = 0; i < w * h; i++)
	{
		++S;
		if (Data[i] == 255)color += 0x01;
		color = color << 1;
		if (S == 8) {
			S = 0;
			IMGSTR += std::to_string(int(color)) + ",";
			color = 0;
			++N;
			if (N == 32) {
				IMGSTR += "\n\t";
				N = 0;
			}
		}
	}
	if (S != 0) {
		IMGSTR += std::to_string(int(color));
	}
	IMGSTR += "\n},";
}


std::vector<unsigned char> resizeImage(unsigned char* inputImage, int inWidth, int inHeight, int newWidth, int newHeight)
{
	std::vector<unsigned char> Img(newWidth * newHeight);
	// 增加权重和像素值变量
	double weight, pixelValue;
	// 根据需要分配新图像数据内存，假设每个像素占用一个字节
	int pixX = TextureW - newWidth;
	int pixY = TextureH - newHeight;
	double widthRatio = static_cast<double>(inWidth) / newWidth;
	double heightRatio = static_cast<double>(inHeight) / newHeight;
	int s = 0;
	for (int i = 0; i < newHeight; ++i)
	{
		for (int j = 0; j < newWidth; ++j)
		{
			int sum = 0;
			double totalWeight = 0.0; // 总权重
			for (int y = static_cast<int>(i * heightRatio); y < static_cast<int>((i + 1) * heightRatio); ++y)
			{
				for (int x = static_cast<int>(j * widthRatio); x < static_cast<int>((j + 1) * widthRatio); ++x)
				{
					// 假设图像数据是灰度图，每个像素占用一个字节
					weight = (1.0 - std::abs(j * widthRatio - x)) * (1.0 - std::abs(i * heightRatio - y)); // 根据距离计算权重
					pixelValue = inputImage[y * inWidth + x]; // 获取像素值
					sum += static_cast<int>(weight * pixelValue);
					totalWeight += weight;
				}
			}
			// 将计算得到的平均值存储到新图像数据中
			//GetPix2(pixX + j, pixY + i, float(static_cast<char>(sum / totalWeight)) / 255);
			Img[s] = static_cast<char>(sum / totalWeight);
			++s;
		}
	}
	return Img;
}

int maxPy = 1000;
int maxa = 0;
int DisplayImage = 0;
bool LinearDisplay = false;
bool PointDisplay = false;
bool RectangleDisplay = true;
unsigned int imgID = 0;
std::vector<std::string> imgVector{};

int JQ_X = 0;
int JQ_Y = 0;
int JQ_W = 100;
int DD_W = 250;
int DD_H = 122;
float BZ = float(DD_H) / DD_W;
int JQ_H = float(JQ_W) * BZ;
bool OnlyDisplayBoxSelections = false;
std::vector<unsigned char> Dimg{};
void OpenImage() {

	int width, height, channels;
	unsigned char* img = stbi_load(("img/" + imgVector[imgID] + ".jpg").c_str(), &width, &height, &channels, 0);
	{
		if (JQ_Y >= height) {
			JQ_Y = 0;
		}
		if (JQ_X >= width) {
			JQ_X = 0;
		}
		if ((JQ_X + JQ_W) >= width) {
			JQ_W = 250;
		}
	}
	if (img != NULL) {
		unsigned char** imgS = new unsigned char* [2];
		for (size_t i = 0; i < 2; i++)
		{
			imgS[i] = new unsigned char[width * height];
		}
		//灰度处理
		ConversionGrayscaleImage(imgS[0], img, width, height, channels);
		//颜色抖动算法
		JQ_H = JQ_W * BZ;
		Dimg = BoxImg(imgS[0], JQ_X, JQ_Y, JQ_W, JQ_H, width);
		Dimg = resizeImage(&Dimg[0], JQ_W, JQ_H, DD_W, DD_H);
		Dithering(&Dimg[0], DD_W, DD_H);

		//显示图像
		if (DisplayImage == 0) {
			for (size_t ix = 0; ix < width; ++ix)
			{
				for (size_t iy = 0; iy < height; ++iy)
				{
					GetPix(ix, iy, &img[(iy * width + ix) * channels]);
				}
			}
		}
		else {
			for (size_t ix = 0; ix < width; ix++)
			{
				for (size_t iy = 0; iy < height; iy++)
				{
					GetPix(ix, iy, (float(imgS[DisplayImage - 1][iy * width + ix]) / 255));
				}
			}
		}

		

		stbi_image_free(img);
		for (size_t i = 0; i < 2; i++)
		{
			delete imgS[i];
		}
		delete[] imgS;
	}
}


void UpdateAtlas() {
	P = (unsigned char*)mTexture->getHOSTImagePointer();
	for (size_t i = 0; i < TextureW * TextureH * 4; i++)
	{
		P[i] = 0;
	}
	mTimer.MomentTiming(u8"处理耗时");
	OpenImage();
	mTimer.MomentEnd();
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

	mTexture = new VulKan::PixelTexture(mDevice, mCommandPool, nullptr, TextureW, TextureH, 4, mSampler);

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


	TOOL::FilePath("img", &imgVector, "jpg");

	UpdateAtlas();

	ImVec2 uv0{ 0,0 }, uv1{ 0.7,0.7 };

	std::vector<char*> ViewMods{
		u8"原图",
		u8"灰度"
	};

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
		if (ImGui::Button(u8"储存")) {
			ImgData(&Dimg[0], DD_W, DD_H);

			std::ofstream file("ImageData.h");
			file << Info_1;
			file << IMGSTR;
			file << Info_2;
			file.clear();
		}
		if (ImGui::BeginCombo(u8"图片", imgVector[imgID].c_str()))
		{
			for (int n = 0; n < imgVector.size(); n++)
			{
				const bool is_selected = (imgID == n);
				if (ImGui::Selectable(imgVector[n].c_str(), is_selected))
					imgID = n;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		if (ImGui::BeginCombo(u8"观察对象", ViewMods[DisplayImage]))
		{
			for (int n = 0; n < ViewMods.size(); n++)
			{
				const bool is_selected = (DisplayImage == n);
				if (ImGui::Selectable(ViewMods[n], is_selected))
					DisplayImage = n;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		for (size_t i = 0; i < mTimer.MomentCount; i++)
		{
			ImGui::Text(u8"%s : %d", mTimer.mMomentNameS[i], mTimer.mMomentTimerS[i]);
		}
		ImGui::Checkbox(u8"只显示框选", &OnlyDisplayBoxSelections);
		ImGui::SliderInt(u8"JQ_X", &JQ_X, 0.0f, TextureW);
		ImGui::SliderInt(u8"JQ_Y", &JQ_Y, 0.0f, TextureH);
		ImGui::SliderInt(u8"JQ_W", &JQ_W, 0.0f, TextureW);
		ImGui::End();

		ImGui::Begin(u8"监视器 ");
		ImGui::SetWindowPos(ImVec2{ Global::mWidth * 0.2f,0 });
		ImGui::SetWindowSize(ImVec2{ float(Global::mWidth * 0.8f), float(Global::mHeight) });
		if (OnlyDisplayBoxSelections) {
			JQ_H = JQ_W * BZ;
			uv0 = { float(JQ_X) / TextureW, float(JQ_Y) / TextureH };
			uv1 = { float(JQ_X + JQ_W) / TextureW, float(JQ_Y + JQ_H) / TextureH };
			ImGui::Image((ImTextureID)mDescriptorSet->getDescriptorSet(mCurrentFrame), ImVec2{ float(Global::mWidth * 0.8f), float(Global::mWidth * 0.8f * BZ) }, uv0, uv1);
		}
		else ImGui::Image((ImTextureID)mDescriptorSet->getDescriptorSet(mCurrentFrame), ImVec2{ float(Global::mWidth * 0.8f), float(Global::mHeight * 0.8f) }, uv0, uv1);
		ImGui::Image((ImTextureID)mDescriptorSet->getDescriptorSet(mCurrentFrame), ImVec2{ float(Global::mHeight * 0.2f) / BZ, float(Global::mHeight * 0.2f) }, { float(TextureW - DD_W) / TextureW, float(TextureH - DD_H) / TextureH }, { 1, 1 });
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

	return 0;
}