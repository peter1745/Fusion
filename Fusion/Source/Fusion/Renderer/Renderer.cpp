#include "FusionPCH.hpp"
#include "Renderer.hpp"
#include "CommandBuffer.hpp"
#include "GraphicsContext.hpp"

#include <tracy/Tracy.hpp>

namespace Fusion {

	static Renderer* s_Instance = nullptr;

	Renderer::Renderer(const Shared<GraphicsContext>& InContext, const RendererInfo& InInfo)
	    : m_Context(InContext), m_Info(InInfo)
	{
		m_FramesInFlight = std::min(m_FramesInFlight, InInfo.TargetSwapChain->GetImageCount());

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
				vkCreateSemaphore(InContext->GetDevice()->GetLogicalDevice(), &SemaphoreInfo, nullptr, &m_ImageAvailableSemaphores[Idx]);
				vkCreateSemaphore(InContext->GetDevice()->GetLogicalDevice(), &SemaphoreInfo, nullptr, &m_RenderFinishedSemaphores[Idx]);
				vkCreateFence(InContext->GetDevice()->GetLogicalDevice(), &FenceInfo, nullptr, &m_ImageFences[Idx]);
			}
		}

		// Create the command allocators
		{
			CommandPoolInfo AllocatorInfo = {};
			AllocatorInfo.InitialListCount = 1;

			for (uint32_t Idx = 0; Idx < m_FramesInFlight; Idx++)
				m_CommandAllocators.push_back(Shared<CommandPool>::Create(InContext->GetDevice(), AllocatorInfo));
		}

		s_Instance = this;
	}

	void Renderer::BeginFrame()
	{
		ZoneScoped;

		vkWaitForFences(m_Context->GetDevice()->GetLogicalDevice(), 1, &m_ImageFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

		uint32_t ImageIndex = m_Info.TargetSwapChain->AcquireNextImage(m_Context->GetDevice()->GetLogicalDevice(), m_ImageAvailableSemaphores[m_CurrentFrame]);

		if (ImageIndex == UINT32_MAX)
		{
			// Failed to acquire next image, possibly because we need to invalidate the swap chain images

			for (auto& ImageSemaphores : m_ImageAvailableSemaphores)
				vkDestroySemaphore(m_Context->GetDevice()->GetLogicalDevice(), ImageSemaphores, nullptr);

			for (auto& RenderSemaphores : m_RenderFinishedSemaphores)
				vkDestroySemaphore(m_Context->GetDevice()->GetLogicalDevice(), RenderSemaphores, nullptr);

			for (auto& ImageFence : m_ImageFences)
				vkDestroyFence(m_Context->GetDevice()->GetLogicalDevice(), ImageFence, nullptr);

			return;
		}

		vkResetFences(m_Context->GetDevice()->GetLogicalDevice(), 1, &m_ImageFences[m_CurrentFrame]);

		for (auto It = m_DestroyQueue.begin(); It != m_DestroyQueue.end();)
		{
			if (It->FrameIndex == m_CurrentFrame)
			{
				FUSION_CORE_VERIFY(It->DestroyFunc, "DestroyFunc isn't valid!");
				It->DestroyFunc();
				It = m_DestroyQueue.erase(It);
			}
			else
			{
				It++;
			}
		}

		m_CommandAllocators[m_CurrentFrame]->Reset();
		m_CommandAllocators[m_CurrentFrame]->GetCommandBuffer(0)->BeginRecording();
	}

	void Renderer::EndFrame()
	{
		ZoneScoped;

		auto* CommandList = m_CommandAllocators[m_CurrentFrame]->GetCommandBuffer(0);

		ImageTransitionInfo TransitionInfo = {};
		TransitionInfo.Image = m_Info.TargetSwapChain->GetImage(m_Info.TargetSwapChain->GetCurrentImage());
		TransitionInfo.OldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		TransitionInfo.NewLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		TransitionInfo.SrcAccessFlag = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		TransitionInfo.DstAccessFlag = 0;
		TransitionInfo.SrcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		TransitionInfo.DstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		TransitionInfo.AspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
		TransitionImage(CommandList->GetBuffer(), TransitionInfo);

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

			const auto& QueueInfo = m_Context->GetDevice().As<Device>()->GetQueueInfo();
			FUSION_CORE_VERIFY(vkQueueSubmit(QueueInfo.Queue, 1, &SubmitInfo, m_ImageFences[m_CurrentFrame]) == VK_SUCCESS);
		}

		m_Info.TargetSwapChain->Present(m_RenderFinishedSemaphores[m_CurrentFrame]);

		m_CurrentFrame = (m_CurrentFrame + 1) % m_FramesInFlight;
	}

	void Renderer::ExecuteCommandLists(const std::vector<CommandBuffer*>& InCommandLists)
	{
		ExecuteCommandLists(InCommandLists, true);
	}

	void Renderer::ExecuteCommandLists(const std::vector<CommandBuffer*>& InCommandLists, bool InShouldSignal)
	{
		ZoneScoped;

		std::vector<VkCommandBuffer> CommandBuffers;
		CommandBuffers.reserve(InCommandLists.size());
		for (auto* CommandList : InCommandLists)
			CommandBuffers.push_back(dynamic_cast<CommandBuffer*>(CommandList)->GetBuffer());

		VkSubmitInfo SubmitInfo = {};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		if (InShouldSignal)
		{
			SubmitInfo.waitSemaphoreCount = 1;
			VkSemaphore WaitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
			VkPipelineStageFlags WaitStageFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			SubmitInfo.pWaitSemaphores = WaitSemaphores;
			SubmitInfo.pWaitDstStageMask = WaitStageFlags;
		}

		SubmitInfo.commandBufferCount = InCommandLists.size();
		SubmitInfo.pCommandBuffers = CommandBuffers.data();

		if (InShouldSignal)
		{
			VkSemaphore SignalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };
			SubmitInfo.signalSemaphoreCount = 1;
			SubmitInfo.pSignalSemaphores = SignalSemaphores;
		}

		VkFence Fence = InShouldSignal ? m_ImageFences[m_CurrentFrame] : VK_NULL_HANDLE;

		const auto& QueueInfo = m_Context->GetDevice()->GetQueueInfo();
		FUSION_CORE_VERIFY(vkQueueSubmit(QueueInfo.Queue, 1, &SubmitInfo, Fence) == VK_SUCCESS);
	}

	void Renderer::Release()
	{
		for (auto& ImageSemaphores : m_ImageAvailableSemaphores)
			vkDestroySemaphore(m_Context->GetDevice()->GetLogicalDevice(), ImageSemaphores, nullptr);

		for (auto& RenderSemaphores : m_RenderFinishedSemaphores)
			vkDestroySemaphore(m_Context->GetDevice()->GetLogicalDevice(), RenderSemaphores, nullptr);

		for (auto& ImageFence : m_ImageFences)
			vkDestroyFence(m_Context->GetDevice()->GetLogicalDevice(), ImageFence, nullptr);

		for (auto& CommandAllocator : m_CommandAllocators)
			CommandAllocator->Release();
		m_CommandAllocators.clear();
	}

	Renderer& Renderer::GetCurrent() { return *s_Instance; }

	CommandQueue::CommandQueue()
	{
		m_CommandBuffer = new uint8_t[10 * 1024 * 1024];
		m_CommandBufferPtr = m_CommandBuffer;
		memset(m_CommandBuffer, 0, 10 * 1024 * 1024);
	}

	CommandQueue::~CommandQueue()
	{
		delete[] m_CommandBuffer;
	}

	void* CommandQueue::AllocateCommand(CommandQueue::CommandFunc InFunc, uint32_t InSize)
	{
		*reinterpret_cast<CommandFunc*>(m_CommandBufferPtr) = InFunc;
		m_CommandBufferPtr += sizeof(CommandFunc);

		*reinterpret_cast<uint32_t*>(m_CommandBufferPtr) = InSize;
		m_CommandBufferPtr += sizeof(uint32_t);

		void* Memory = m_CommandBufferPtr;
		m_CommandBufferPtr += InSize;

		m_CommandCount++;
		return Memory;
	}

	void CommandQueue::Execute(CommandBuffer* InCmdList)
	{
		uint8_t* Buffer = m_CommandBuffer;

		for (uint32_t Idx = 0; Idx < m_CommandCount; Idx++)
		{
			CommandFunc Func = *reinterpret_cast<CommandFunc*>(Buffer);
			Buffer += sizeof(CommandFunc);

			uint32_t Size = *reinterpret_cast<uint32_t*>(Buffer);
			Buffer += sizeof(uint32_t);

			Func(Buffer, InCmdList);
			Buffer += Size;
		}

		m_CommandBufferPtr = m_CommandBuffer;
		m_CommandCount = 0;
	}

}