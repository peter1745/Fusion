#pragma once

#include "VulkanDevice.h"

namespace Fusion {

	class VulkanRenderer
	{
	public:
		VulkanRenderer();
		~VulkanRenderer();

		void BeginDraw();
		void Draw(float InWidth, float InHeight);
		void EndDraw();
		void Submit();

	public:
		static VulkanRenderer& Get();

	private:
		VkCommandPool m_CommandPool = VK_NULL_HANDLE;
		VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;

	};

}
