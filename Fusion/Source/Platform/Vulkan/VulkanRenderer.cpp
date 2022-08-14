#include "FusionPCH.h"
#include "VulkanRenderer.h"
#include "VulkanContext.h"

namespace Fusion {

	static VulkanRenderer* s_Instance = nullptr;

	VulkanRenderer::VulkanRenderer()
	{
		FUSION_CORE_VERIFY(s_Instance == nullptr, "Cannot instantiate Vulkan Renderer multiple times!");
		s_Instance = this;

		Shared<VulkanDevice> Device = VulkanContext::Get().GetDevice();

		// Create Command Pool and Command Buffers
		{
			VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
			CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			CommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			CommandPoolCreateInfo.queueFamilyIndex = Device->GetQueueIndicies().GraphicsQueue;

			FUSION_CORE_VERIFY(vkCreateCommandPool(Device->GetLogicalDevice(), &CommandPoolCreateInfo, nullptr, &m_CommandPool) == VK_SUCCESS);

			VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
			CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			CommandBufferAllocateInfo.commandPool = m_CommandPool;
			CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			CommandBufferAllocateInfo.commandBufferCount = 1;

			FUSION_CORE_VERIFY(vkAllocateCommandBuffers(Device->GetLogicalDevice(), &CommandBufferAllocateInfo, &m_CommandBuffer) == VK_SUCCESS);
		}
	}

	VulkanRenderer::~VulkanRenderer()
	{
		Shared<VulkanDevice> Device = VulkanContext::Get().GetDevice();
		vkDestroyCommandPool(Device->GetLogicalDevice(), m_CommandPool, nullptr);

		s_Instance = nullptr;
	}

	void VulkanRenderer::BeginDraw()
	{
		auto Swapchain = VulkanContext::Get().GetSwapchain();
		Swapchain->AquireNextFrame();

		vkResetCommandBuffer(m_CommandBuffer, 0);

		VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
		CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CommandBufferBeginInfo.flags = 0;
		FUSION_CORE_VERIFY(vkBeginCommandBuffer(m_CommandBuffer, &CommandBufferBeginInfo) == VK_SUCCESS);

		VkRenderPassBeginInfo RenderPassBeginInfo = {};
		RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassBeginInfo.renderPass = Swapchain->GetRenderPass();
		RenderPassBeginInfo.framebuffer = Swapchain->GetFramebuffer(Swapchain->GetCurrentFrameIndex());
		RenderPassBeginInfo.renderArea.offset = { 0, 0 };
		RenderPassBeginInfo.renderArea.extent = Swapchain->GetSwapchainExtent();

		VkClearValue ClearValue = { {{ 1.0f, 0.0f, 1.0f, 1.0f }} };
		RenderPassBeginInfo.clearValueCount = 1;
		RenderPassBeginInfo.pClearValues = &ClearValue;

		vkCmdBeginRenderPass(m_CommandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanContext::Get().GetGraphicsPipeline()->GetPipeline());
	}

	void VulkanRenderer::EndDraw()
	{
		vkCmdEndRenderPass(m_CommandBuffer);
		FUSION_CORE_VERIFY(vkEndCommandBuffer(m_CommandBuffer) == VK_SUCCESS);
	}
	
	void VulkanRenderer::Submit()
	{
		auto Swapchain = VulkanContext::Get().GetSwapchain();

		VkSubmitInfo SubmitInfo = {};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkSemaphore WaitSemaphores[] = { Swapchain->GetImageAvailableSemaphore() };
		VkPipelineStageFlags PipelineStageFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		SubmitInfo.waitSemaphoreCount = 1;
		SubmitInfo.pWaitSemaphores = WaitSemaphores;
		SubmitInfo.pWaitDstStageMask = PipelineStageFlags;
		SubmitInfo.commandBufferCount = 1;
		SubmitInfo.pCommandBuffers = &m_CommandBuffer;

		VkSemaphore SignalSemaphores[] = { Swapchain->GetRenderFinishedSemaphore() };
		SubmitInfo.signalSemaphoreCount = 1;
		SubmitInfo.pSignalSemaphores = SignalSemaphores;

		auto Device = VulkanContext::Get().GetDevice();
		FUSION_CORE_VERIFY(vkQueueSubmit(Device->GetGraphicsQueue(), 1, &SubmitInfo, Swapchain->GetFrameInFlightFence()) == VK_SUCCESS);

		Swapchain->SwapBuffers();
	}

	void VulkanRenderer::Draw(float InWidth, float InHeight)
	{
		auto Swapchain = VulkanContext::Get().GetSwapchain();
	
		VkExtent2D SwapchainExtent = Swapchain->GetSwapchainExtent();

		VkViewport Viewport = {};
		Viewport.x = 0.0f;
		Viewport.y = 0.0f;
		Viewport.width = SwapchainExtent.width;
		Viewport.height = SwapchainExtent.height;
		Viewport.minDepth = 0.0f;
		Viewport.maxDepth = 1.0f;

		VkRect2D Scissor = {};
		Scissor.offset = { 0, 0 };
		Scissor.extent = SwapchainExtent;

		vkCmdSetViewport(m_CommandBuffer, 0, 1, &Viewport);
		vkCmdSetScissor(m_CommandBuffer, 0, 1, &Scissor);
		vkCmdDraw(m_CommandBuffer, 3, 1, 0, 0);
	}

	VulkanRenderer& VulkanRenderer::Get() { return *s_Instance; }

}
