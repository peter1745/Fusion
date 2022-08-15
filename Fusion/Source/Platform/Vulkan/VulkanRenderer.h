#pragma once

#include "VulkanDevice.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"

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
		std::vector<VkCommandPool> m_CommandPools;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		Shared<VulkanRenderPass> m_SwapchainRenderPass;
		Shared<VulkanPipeline> m_TrianglePipeline = nullptr;
	};

}
