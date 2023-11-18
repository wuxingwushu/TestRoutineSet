#pragma once
#include "GUI.h"
#include "../Vulkan/Window.h"
#include "../Vulkan/device.h"
#include "../Vulkan/renderPass.h"
#include "../Vulkan/commandPool.h"
#include "../Vulkan/commandBuffer.h"

#include "../GlobalVariable.h"

namespace GAME {

	class ImGuiInterFace
	{
		struct ChatBoxStr {
			std::string str;
			clock_t timr;
		};
	public:
		ImGuiInterFace(
			VulKan::Device* device, 
			VulKan::Window* Win, 
			ImGui_ImplVulkan_InitInfo Info, 
			VulKan::RenderPass* Pass,
			VulKan::CommandBuffer* commandbuffer,
			int FormatCount
		);

		void StructureImGuiInterFace();//构建

		~ImGuiInterFace();

		void InterFace();//显示

		bool GetInterFaceBool() {//获取显示状态
			return InterFaceBool;
		}

		void SetInterFaceBool() {//显示状态取反
			InterFaceBool = !InterFaceBool;
		}

		VkCommandBuffer GetCommandBuffer(int i, VkCommandBufferInheritanceInfo info);

		ImFont* Font; //字体
	private:
		VkDescriptorPool			g_DescriptorPool = VK_NULL_HANDLE;//给 ImGui 创建的 DescriptorPool 记得销毁
		int							g_MinImageCount = 3;
		int mFormatCount;
		VulKan::Window* mWindown{ nullptr };
		VulKan::Device* mDevice{ nullptr };
		ImGui_ImplVulkan_InitInfo mInfo;
		VulKan::RenderPass* mRenderPass;
		
		VulKan::CommandBuffer* mCommandBuffer;

		bool InterFaceBool = true;//是否显示界面

		VulKan::CommandPool** ImGuiCommandPoolS;
		VulKan::CommandBuffer** ImGuiCommandBufferS;
	};
}
