#pragma once
#include "device.h"

namespace VulKan {

	class CommandPool {
	public:
		CommandPool(Device* device, VkCommandPoolCreateFlagBits flag = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		~CommandPool();

		[[nodiscard]] auto getCommandPool() const noexcept { return mCommandPool; }

	private:
		VkCommandPool mCommandPool{ VK_NULL_HANDLE };
		Device* mDevice{ nullptr };
	};
}