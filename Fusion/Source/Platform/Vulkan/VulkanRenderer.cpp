#include "FusionPCH.h"
#include "VulkanRenderer.h"
#include "VulkanContext.h"

#include "Fusion/Renderer/RenderData.h"

namespace Fusion {

	static VulkanRenderer* s_Instance = nullptr;

	static Vertex s_TriangleVertices[] = {
		{ {  0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Top-Right
		{ {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Bottom-Right
		{ { -0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Bottom-Left
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // Top-Left
	};

	static uint32_t s_TriangleIndices[] = { 0, 1, 2, 2, 3, 0 };

	static VkImageSubresourceRange GetImageSubresourceRange(VkImageAspectFlags InAspectFlags)
	{
		VkImageSubresourceRange result = {};
		result.aspectMask = InAspectFlags;
		result.baseMipLevel = 0;
		result.levelCount = VK_REMAINING_MIP_LEVELS;
		result.baseArrayLayer = 0;
		result.layerCount = VK_REMAINING_ARRAY_LAYERS;
		return result;
	}

	VulkanRenderer::VulkanRenderer()
	{
		FUSION_CORE_VERIFY(s_Instance == nullptr, "Cannot instantiate Vulkan Renderer multiple times!");
		s_Instance = this;

		Shared<VulkanDevice> Device = VulkanContext::Get().GetDevice();
		Shared<VulkanSwapchain> Swapchain = VulkanContext::Get().GetSwapchain();

		// Swapchain Render Pass
		{
			RenderPassSpecification RenderPassSpec = {};
			RenderPassSpec.IsSwapchainTarget = true;
			m_SwapchainRenderPass = Shared<VulkanRenderPass>::Create(RenderPassSpec);
		}

		// Create Triangle Pipeline
		{
			PipelineSpecification TrianglePipelineSpec = {};
			ShaderSpecification ShaderSpec = { "Resources/Shaders/VertexShader.spv", "Resources/Shaders/FragmentShader.spv" };

			TrianglePipelineSpec.PipelineShader = Shared<VulkanShader>::Create(ShaderSpec, Device);
			TrianglePipelineSpec.ColorAttachmentFormat = Swapchain->GetSwapchainFormat();
			TrianglePipelineSpec.Layout = VertexBufferLayout({
				VertexBufferAttribute{ 0, ShaderDataType::Float3, offsetof(Vertex, Position) },
				VertexBufferAttribute{ 1, ShaderDataType::Float4, offsetof(Vertex, Color) }
			});
			m_TrianglePipeline = Shared<VulkanPipeline>::Create(TrianglePipelineSpec, Device);
		}

		// Create Command Pool and Command Buffers
		{
			uint32_t MaxFramesInFlight = Swapchain->GetMaxFramesInFlight();
			m_CommandPools.resize(MaxFramesInFlight);
			m_CommandBuffers.resize(MaxFramesInFlight);
			for (uint32_t i = 0; i < MaxFramesInFlight; i++)
			{
				VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
				CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				CommandPoolCreateInfo.flags = 0;
				CommandPoolCreateInfo.queueFamilyIndex = Device->GetQueueIndicies().GraphicsQueue;
				FUSION_CORE_VERIFY(vkCreateCommandPool(Device->GetLogicalDevice(), &CommandPoolCreateInfo, nullptr, &m_CommandPools[i]) == VK_SUCCESS);

				VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
				CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				CommandBufferAllocateInfo.commandPool = m_CommandPools[i];
				CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				CommandBufferAllocateInfo.commandBufferCount = 1;
				FUSION_CORE_VERIFY(vkAllocateCommandBuffers(Device->GetLogicalDevice(), &CommandBufferAllocateInfo, &m_CommandBuffers[i]) == VK_SUCCESS);
			}
		}

		m_TriangleVertexBuffer = Shared<VulkanVertexBuffer>::Create(4 * sizeof(Vertex), s_TriangleVertices);
		m_TriangleIndexBuffer = Shared<VulkanIndexBuffer>::Create(6, s_TriangleIndices);
	}

	VulkanRenderer::~VulkanRenderer()
	{
		Shared<VulkanSwapchain> Swapchain = VulkanContext::Get().GetSwapchain();
		Shared<VulkanDevice> Device = VulkanContext::Get().GetDevice();

		vkDeviceWaitIdle(Device->GetLogicalDevice());

		for (uint32_t i = 0; i < Swapchain->GetMaxFramesInFlight(); i++)
			vkDestroyCommandPool(Device->GetLogicalDevice(), m_CommandPools[i], nullptr);

		s_Instance = nullptr;
	}

	void VulkanRenderer::BeginDraw()
	{
		auto Device = VulkanContext::Get().GetDevice();
		auto Swapchain = VulkanContext::Get().GetSwapchain();

		// If we fail to aquire the next image we need to bail
		if (!Swapchain->AquireNextFrame())
			return;

		uint32_t CurrentFrame = Swapchain->GetCurrentFrame();

		vkResetCommandPool(Device->GetLogicalDevice(), m_CommandPools[CurrentFrame], 0);
		VkCommandBuffer CurrentCommandBuffer = m_CommandBuffers[CurrentFrame];

		VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
		CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CommandBufferBeginInfo.flags = 0;
		FUSION_CORE_VERIFY(vkBeginCommandBuffer(CurrentCommandBuffer, &CommandBufferBeginInfo) == VK_SUCCESS);

		VkImageMemoryBarrier2 ImageMemoryBarrier = {};
		ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		ImageMemoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
		ImageMemoryBarrier.srcAccessMask = VK_ACCESS_2_NONE;
		ImageMemoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_CLEAR_BIT;
		ImageMemoryBarrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		ImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		ImageMemoryBarrier.image = Swapchain->GetCurrentImage();
		ImageMemoryBarrier.subresourceRange = GetImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

		VkDependencyInfo DependencyInfo = {};
		DependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		DependencyInfo.dependencyFlags = 0;
		DependencyInfo.imageMemoryBarrierCount = 1;
		DependencyInfo.pImageMemoryBarriers = &ImageMemoryBarrier;
		vkCmdPipelineBarrier2(CurrentCommandBuffer, &DependencyInfo);

		m_SwapchainRenderPass->Begin(CurrentCommandBuffer);
		vkCmdBindPipeline(CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_TrianglePipeline->GetPipeline());

		m_TriangleVertexBuffer->Bind(CurrentCommandBuffer);
		m_TriangleIndexBuffer->Bind(CurrentCommandBuffer);
	}

	void VulkanRenderer::EndDraw()
	{
		auto Swapchain = VulkanContext::Get().GetSwapchain();
		VkCommandBuffer CurrentCommandBuffer = m_CommandBuffers[Swapchain->GetCurrentFrame()];
		
		m_SwapchainRenderPass->End(CurrentCommandBuffer);

		VkImageMemoryBarrier2 ImageMemoryBarrier = {};
		ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		ImageMemoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		ImageMemoryBarrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
		ImageMemoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
		ImageMemoryBarrier.dstAccessMask = VK_PIPELINE_STAGE_2_NONE;
		ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		ImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		ImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		ImageMemoryBarrier.image = Swapchain->GetCurrentImage();
		ImageMemoryBarrier.subresourceRange = GetImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

		VkDependencyInfo DependencyInfo = {};
		DependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		DependencyInfo.dependencyFlags = 0;
		DependencyInfo.imageMemoryBarrierCount = 1;
		DependencyInfo.pImageMemoryBarriers = &ImageMemoryBarrier;
		vkCmdPipelineBarrier2(CurrentCommandBuffer, &DependencyInfo);

		FUSION_CORE_VERIFY(vkEndCommandBuffer(CurrentCommandBuffer) == VK_SUCCESS);
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
		SubmitInfo.pCommandBuffers = &m_CommandBuffers[Swapchain->GetCurrentFrame()];

		VkSemaphore SignalSemaphores[] = { Swapchain->GetRenderFinishedSemaphore() };
		SubmitInfo.signalSemaphoreCount = 1;
		SubmitInfo.pSignalSemaphores = SignalSemaphores;

		auto Device = VulkanContext::Get().GetDevice();
		FUSION_CORE_VERIFY(vkQueueSubmit(Device->GetGraphicsQueue(), 1, &SubmitInfo, Swapchain->GetFrameInFlightFence()) == VK_SUCCESS);

		Swapchain->SwapBuffers();
	}

	VkCommandBuffer VulkanRenderer::GetCurrentCommandBuffer() const
	{
		auto Swapchain = VulkanContext::Get().GetSwapchain();
		return m_CommandBuffers[Swapchain->GetCurrentFrame()];
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

		VkCommandBuffer CurrentCommandBuffer = m_CommandBuffers[Swapchain->GetCurrentFrame()];
		vkCmdSetViewport(CurrentCommandBuffer, 0, 1, &Viewport);
		vkCmdSetScissor(CurrentCommandBuffer, 0, 1, &Scissor);
		vkCmdDrawIndexed(CurrentCommandBuffer, m_TriangleIndexBuffer->GetCount(), 1, 0, 0, 0);
	}

	VulkanRenderer& VulkanRenderer::Get() { return *s_Instance; }

}

