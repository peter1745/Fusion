#include "FusionPCH.hpp"
#include "VulkanRenderer.hpp"
#include "VulkanCommandList.hpp"
#include "VulkanContext.hpp"

namespace Fusion {

	VulkanRenderer::VulkanRenderer(const RendererInfo& InInfo)
	{
		m_Context = GraphicsContext::Get<VulkanContext>();
		m_SwapChain = InInfo.TargetSwapChain.As<VulkanSwapChain>();

		m_FramesInFlight = std::min(m_FramesInFlight, m_SwapChain->GetImageCount());

		auto Device = m_Context->GetDevice().As<VulkanDevice>();

		// Create frame semaphores and fences
		{
			m_ImageAvailableSemaphores.resize(m_FramesInFlight);
			m_RenderFinishedSemaphores.resize(m_FramesInFlight);
			m_ImageFences.resize(m_FramesInFlight);

			VkSemaphoreCreateInfo SemaphoreInfo = {};
			SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo FenceInfo = {};
			FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			for (uint32_t Idx = 0; Idx < m_FramesInFlight; Idx++)
			{
				vkCreateSemaphore(Device->GetLogicalDevice(), &SemaphoreInfo, nullptr, &m_ImageAvailableSemaphores[Idx]);
				vkCreateSemaphore(Device->GetLogicalDevice(), &SemaphoreInfo, nullptr, &m_RenderFinishedSemaphores[Idx]);
				vkCreateFence(Device->GetLogicalDevice(), &FenceInfo, nullptr, &m_ImageFences[Idx]);
			}
		}

		// Create the command allocators
		{
			CommandAllocatorInfo AllocatorInfo = {};
			AllocatorInfo.ListType = ECommandListType::Direct;
			AllocatorInfo.InitialListCount = 1;

			for (uint32_t Idx = 0; Idx < m_FramesInFlight; Idx++)
				m_CommandAllocators.push_back(Shared<VulkanCommandAllocator>::Create(Device, AllocatorInfo));
		}
	}

	void VulkanRenderer::BeginFrame()
	{
		m_CurrentFrame = (m_CurrentFrame + 1) % m_FramesInFlight;

		auto Device = m_Context->GetDevice().As<VulkanDevice>();

		vkWaitForFences(Device->GetLogicalDevice(), 1, &m_ImageFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

		uint32_t ImageIndex = m_SwapChain->AcquireNextImage(Device->GetLogicalDevice(), m_ImageAvailableSemaphores[m_CurrentFrame]);

		if (ImageIndex == UINT32_MAX)
		{
			// Failed to acquire next image, possibly because we need to invalidate the swap chain images

			for (auto& ImageSemaphores : m_ImageAvailableSemaphores)
				vkDestroySemaphore(Device->GetLogicalDevice(), ImageSemaphores, nullptr);

			for (auto& RenderSemaphores : m_RenderFinishedSemaphores)
				vkDestroySemaphore(Device->GetLogicalDevice(), RenderSemaphores, nullptr);

			for (auto& ImageFence : m_ImageFences)
				vkDestroyFence(Device->GetLogicalDevice(), ImageFence, nullptr);

			return;
		}

		vkResetFences(Device->GetLogicalDevice(), 1, &m_ImageFences[m_CurrentFrame]);

		m_CommandAllocators[m_CurrentFrame]->Reset();
		m_CommandAllocators[m_CurrentFrame]->GetCommandList(0)->BeginRecording();
	}

	void VulkanRenderer::EndFrame()
	{
		auto* CommandList = dynamic_cast<VulkanCommandList*>(m_CommandAllocators[m_CurrentFrame]->GetCommandList(0));

		CommandList->EndRecording();

		// Submit command list (Deliberately not using ExecuteCommandLists)
		{
			VkSemaphore WaitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
			VkPipelineStageFlags WaitStageFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			VkSemaphore SignalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };

			VkCommandBuffer CommandBuffer = CommandList->GetBuffer();

			VkSubmitInfo SubmitInfo = {};
			SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			SubmitInfo.waitSemaphoreCount = 1;
			SubmitInfo.pWaitSemaphores = WaitSemaphores;
			SubmitInfo.pWaitDstStageMask = WaitStageFlags;
			SubmitInfo.commandBufferCount = 1;
			SubmitInfo.pCommandBuffers = &CommandBuffer;

			SubmitInfo.signalSemaphoreCount = 1;
			SubmitInfo.pSignalSemaphores = SignalSemaphores;

			const auto& QueueInfo =	m_Context->GetDevice().As<VulkanDevice>()->GetQueueInfo();
			FUSION_CORE_VERIFY(vkQueueSubmit(QueueInfo.Queue, 1, &SubmitInfo, m_ImageFences[m_CurrentFrame]) == VK_SUCCESS);
		}

		m_SwapChain->Present(m_RenderFinishedSemaphores[m_CurrentFrame]);
	}

	void VulkanRenderer::ExecuteCommandLists(const std::vector<CommandList*>& InCommandLists)
	{
		auto Ctx = GraphicsContext::Get<VulkanContext>();

		std::vector<VkCommandBuffer> CommandBuffers;
		CommandBuffers.reserve(InCommandLists.size());
		for (auto* CommandList : InCommandLists)
			CommandBuffers.push_back(dynamic_cast<VulkanCommandList*>(CommandList)->GetBuffer());

		VkSubmitInfo SubmitInfo = {};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		SubmitInfo.waitSemaphoreCount = 1;
		VkSemaphore WaitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
		VkPipelineStageFlags WaitStageFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		SubmitInfo.pWaitSemaphores = WaitSemaphores;
		SubmitInfo.pWaitDstStageMask = WaitStageFlags;
		SubmitInfo.commandBufferCount = InCommandLists.size();
		SubmitInfo.pCommandBuffers = CommandBuffers.data();

		VkSemaphore SignalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };
		SubmitInfo.signalSemaphoreCount = 1;
		SubmitInfo.pSignalSemaphores = SignalSemaphores;

		const auto& QueueInfo = Ctx->GetDevice().As<VulkanDevice>()->GetQueueInfo();
		FUSION_CORE_VERIFY(vkQueueSubmit(QueueInfo.Queue, 1, &SubmitInfo, m_ImageFences[m_CurrentFrame]) == VK_SUCCESS);
	}

	void VulkanRenderer::Release()
	{
		auto Device = m_Context->GetDevice().As<VulkanDevice>();

		for (auto& ImageSemaphores : m_ImageAvailableSemaphores)
			vkDestroySemaphore(Device->GetLogicalDevice(), ImageSemaphores, nullptr);

		for (auto& RenderSemaphores : m_RenderFinishedSemaphores)
			vkDestroySemaphore(Device->GetLogicalDevice(), RenderSemaphores, nullptr);

		for (auto& ImageFence : m_ImageFences)
			vkDestroyFence(Device->GetLogicalDevice(), ImageFence, nullptr);

		m_CommandAllocators.clear();
	}

}