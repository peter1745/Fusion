#pragma once

#include "Fusion/Renderer/Renderer.hpp"

#include "GraphicsContext.hpp"
#include "SwapChain.hpp"

namespace Fusion {

	struct RendererInfo
	{
		Shared<SwapChain> TargetSwapChain;
	};

	class CommandQueue
	{
	public:
		typedef void (*CommandFunc)(void*, CommandBuffer*);

		CommandQueue();
		~CommandQueue();

		void* AllocateCommand(CommandFunc InFunc, uint32_t InSize);
		void Execute(CommandBuffer* InCmdList);

	private:
		uint8_t* m_CommandBuffer;
		uint8_t* m_CommandBufferPtr;
		uint32_t m_CommandCount = 0;
	};

	struct ResourceDestroyInfo
	{
		uint32_t FrameIndex;
		std::function<void()> DestroyFunc;
	};

	class Renderer
	{
	public:
		using ResourceDestroyQueue = std::vector<ResourceDestroyInfo>;
	public:
		Renderer(const Shared<GraphicsContext>& InContext, const RendererInfo& InInfo);
		~Renderer() = default;

		void BeginFrame();
		void EndFrame();

		void ExecuteCommandLists(const std::vector<CommandBuffer*>& InCommandLists);
		void ExecuteCommandLists(const std::vector<CommandBuffer*>& InCommandLists, bool InShouldSignal);

		Shared<CommandPool> GetCurrentCommandAllocator() const { return m_CommandAllocators[m_CurrentFrame]; }
		CommandBuffer* GetCurrentCommandList() const { return m_CommandAllocators[m_CurrentFrame]->GetCommandBuffer(0); }

		[[nodiscard]] uint32_t GetFramesInFlight() const { return m_FramesInFlight; }
		[[nodiscard]] uint32_t GetCurrentFrame() const { return m_CurrentFrame; }

		void SubmitResourceForDestruction(uint32_t InFrameOffset, const std::function<void()>& InFunc)
		{
			auto& DestroyInfo = m_DestroyQueue.emplace_back();
			DestroyInfo.FrameIndex = (m_CurrentFrame + InFrameOffset) % m_FramesInFlight;
			DestroyInfo.DestroyFunc = InFunc;
		}

		void ExecuteImmediate(const std::function<void(CommandBuffer*)>& InFunc, bool InWaitBeforeExit = false);

		void Release();

	public:
		static Renderer& GetCurrent();

	private:
		Shared<GraphicsContext> m_Context = nullptr;
		RendererInfo m_Info;

		uint32_t m_CurrentFrame = 0;
		uint32_t m_FramesInFlight = 3;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_ImageFences;

		std::vector<Shared<CommandPool>> m_CommandAllocators;

		ResourceDestroyQueue m_DestroyQueue;
	};

}
