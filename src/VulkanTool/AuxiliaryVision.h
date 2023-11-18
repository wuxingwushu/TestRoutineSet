#pragma once
#include "../Vulkan/buffer.h"
#include "../Vulkan/device.h"
#include "../Vulkan/descriptorSet.h"
#include "PipelineS.h"
#include "../Vulkan/swapChain.h"
#include "../Vulkan/renderPass.h"
#include "PixelTexture.h"
#include "../Tool/ContinuousMap.h"
//矩阵计算库
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace VulKan {

	struct AuxiliarySpot
	{
		glm::vec3 Pos{};
		glm::vec4 Color{};
	};

	struct AuxiliaryLineData
	{
		glm::vec2* Head{ nullptr };
		glm::vec2* Tail{ nullptr };
		glm::vec4 Color{};
	};

	struct AuxiliaryForceData
	{
		glm::vec2* pos{ nullptr };
		glm::vec2* Force{ nullptr };
		glm::vec4 Color{};
	};

	struct AuxiliarySpotData
	{
		glm::vec2* pos{ nullptr };
		glm::vec4 Color{};
	};


	typedef unsigned int (*AuxiliaryDataHandle)(AuxiliarySpot* P, void* D, unsigned int Size);
	struct StaticAuxiliaryData
	{
		void* Pointer = nullptr;
		unsigned int Size = 0;
		AuxiliaryDataHandle Function = nullptr;
	};

	

	class AuxiliaryVision
	{
	public:
		AuxiliaryVision(Device* device, PipelineS* P, const unsigned int number);
		~AuxiliaryVision();

		//初始化描述符
		void initUniformManager(
			int frameCount, //GPU画布的数量
			std::vector<Buffer*> VPMstdBuffer//玩家相机的变化矩阵 
		);

		//初次录制 CommandBuffer
		void RecordingCommandBuffer(RenderPass* R, SwapChain* S) {
			wRenderPass = R;
			wSwapChain = S;
			initCommandBuffer();
		}

		//刷新 CommandBuffer
		void initCommandBuffer();

		//获取某帧的 CommandBuffer
		void GetCommandBuffer(std::vector<VkCommandBuffer>* Vector, unsigned int F) {
			Vector->push_back(mCommandBuffer[F]->getCommandBuffer());
		}

		//开始录制辅助视觉
		void Begin();

		//画线
		void Line(glm::vec3 Vertex1, glm::vec4 color1, glm::vec3 Vertex2, glm::vec4 color2) {
			LinePointerHOST->Pos = Vertex1;
			LinePointerHOST->Color = color1;
			++LinePointerHOST;
			LinePointerHOST->Pos = Vertex2;
			LinePointerHOST->Color = color2;
			++LinePointerHOST;
			LineNumber += 2;
		};

		//画点
		void Spot(glm::vec3 pos, glm::vec4 color) {
			SpotPointerHOST->Pos = pos;
			SpotPointerHOST->Color = color;
			++SpotPointerHOST;
			++SpotNumber;
		};

		//结束录制辅助视觉
		void End();

		//一次性线段（录制外使用）
		void AddLine(glm::vec3 Vertex1, glm::vec3 Vertex2, glm::vec4 color) {
			LineVertex.push_back({ Vertex1, color });
			LineVertex.push_back({ Vertex2, color });
		};
		//一次性点（录制外使用）
		void AddSpot(glm::vec3 pos, glm::vec4 color) {
			SpotVertex.push_back({ pos, color });
		};

		//静态线段（录制外使用）
		void AddStaticLine(glm::vec3 Vertex1, glm::vec3 Vertex2, glm::vec4 color) {
			StaticLineVertex.push_back({ Vertex1, color });
			StaticLineVertex.push_back({ Vertex2, color });
			StaticLineVertexUpData = true;
		};
		//静态点（录制外使用）
		void AddStaticSpot(glm::vec3 pos, glm::vec4 color) {
			StaticSpotVertex.push_back({ pos, color });
			StaticSpotVertexUpData = true;
		};

		//动态线段
		auto GetContinuousLine() { return ContinuousAuxiliaryLine; }
		//动态线段（力）
		auto GetContinuousForce() { return ContinuousAuxiliaryForce; }
		//动态点
		auto GetContinuousSpot() { return ContinuousAuxiliarySpot; }

		//静态线段
		auto GetContinuousStaticLine() { return StaticContinuousAuxiliaryLine; }
		//静态点
		auto GetContinuousStaticSpot() { return StaticContinuousAuxiliarySpot; }
		//静态数据更新
		void OpenStaticSpotUpData() { StaticSpotUpData = true; }
		void OpenStaticLineUpData() { StaticLineUpData = true; }
	private://线段
		AuxiliarySpot* LinePointerHOST = nullptr;
		unsigned int LineNumber = 0;//当前帧的数量
		unsigned int LineMax = 0;//上一帧的数量
		ContinuousMap<glm::vec2*, AuxiliaryLineData>* ContinuousAuxiliaryLine = nullptr;//两点连线（动态）
		ContinuousMap<glm::vec2*, AuxiliaryForceData>* ContinuousAuxiliaryForce = nullptr;//点上的向量（动态）
		ContinuousMap<void*, StaticAuxiliaryData>* StaticContinuousAuxiliaryLine = nullptr;//线段集（静态）
		bool StaticLineUpData = false;//静态线段是否需要更新
		unsigned int StaticLineDeviation = 0;//静态数据偏移量
		std::vector<AuxiliarySpot> LineVertex{};//单纯的位置连线（一次性）
		bool StaticLineVertexUpData = false;//静态线段是否需要更新
		unsigned int StaticLineVertexDeviation = 0;//静态数据偏移量
		std::vector<AuxiliarySpot> StaticLineVertex{};//单纯的位置连线（静态）
		Buffer* AuxiliaryLineS{ nullptr };//线段缓存
	private://点
		AuxiliarySpot* SpotPointerHOST = nullptr;
		unsigned int SpotNumber = 0;//当前帧的数量
		unsigned int SpotMax = 0;//上一帧的数量
		ContinuousMap<glm::vec2*, AuxiliarySpotData>* ContinuousAuxiliarySpot = nullptr;//点集（动态）
		ContinuousMap<void*, StaticAuxiliaryData>* StaticContinuousAuxiliarySpot = nullptr;//点集（静态）
		bool StaticSpotUpData = false;//静态点是否需要更新
		unsigned int StaticSpotDeviation = 0;//静态数据偏移量
		std::vector<AuxiliarySpot> SpotVertex{};//单纯的位置点（一次性）
		bool StaticSpotVertexUpData = false;//静态点是否需要更新
		unsigned int StaticSpotVertexDeviation = 0;//静态数据偏移量
		std::vector<AuxiliarySpot> StaticSpotVertex{};//单纯的位置点（静态）
		Buffer* AuxiliarySpotS{ nullptr };//点缓存

	private:
		DescriptorPool* mDescriptorPool{ nullptr };
		DescriptorSet* mDescriptorSetLine{ nullptr };
		DescriptorSet* mDescriptorSetSpot{ nullptr };
		CommandPool* mCommandPool{ nullptr };
		CommandBuffer** mCommandBuffer{ nullptr };

	private://储存
		const unsigned int Number;
		Device* wDevice{ nullptr };
		PipelineS* wPipelineS{ nullptr };
		SwapChain* wSwapChain{ nullptr };
		RenderPass* wRenderPass{ nullptr };
	};

}