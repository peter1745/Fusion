#pragma once

#include "Device.hpp"
#include "SwapChain.hpp"
#include "CommandList.hpp"

namespace Fusion {

	struct RendererInfo
	{
		Shared<SwapChain> TargetSwapChain;
	};

	class CommandQueue
	{
	public:
		typedef void (*CommandFunc)(void*, CommandList*);

		CommandQueue();
		~CommandQueue();

		void* AllocateCommand(CommandFunc InFunc, uint32_t InSize);
		void Execute(CommandList* InCmdList);

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
		virtual ~Renderer() = default;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void ExecuteCommandLists(const std::vector<CommandList*>& InCommandLists) = 0;

		virtual Shared<CommandAllocator> GetCurrentCommandAllocator() const = 0;
		virtual CommandList* GetCurrentCommandList() const = 0;

		virtual uint32_t GetFramesInFlight() const = 0;
		virtual uint32_t GetCurrentFrame() const = 0;

		virtual void SubmitResourceForDestruction(uint32_t InFrameOffset, const std::function<void()>& InFunc) = 0;

		virtual void Release() = 0;

	public:
		static Unique<Renderer> Create(const RendererInfo& InInfo);

		static Renderer& GetCurrent();
	};

}
