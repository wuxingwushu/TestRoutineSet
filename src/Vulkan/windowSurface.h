#pragma once
#include "instance.h"
#include "window.h"

namespace VulKan {

	class WindowSurface {
	public:
		//创建Surface，让VulKan和窗口链接起来（适配win,安卓,等等不同设备）
		WindowSurface(Instance* instance, Window* window);

		~WindowSurface();

		//获取Surface的指针
		[[nodiscard]] VkSurfaceKHR getSurface() const noexcept { return mWindowSurface; }

	private:
		VkSurfaceKHR mWindowSurface{ VK_NULL_HANDLE };
		Instance* mInstance{ nullptr };
	};
}
