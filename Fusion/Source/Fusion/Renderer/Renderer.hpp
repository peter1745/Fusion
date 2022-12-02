#pragma once

#include "Device.hpp"
#include "SwapChain.hpp"
#include "CommandList.hpp"

namespace Fusion {

	struct RendererInfo
	{
		Shared<SwapChain> TargetSwapChain;
	};

	class Renderer
	{
	public:
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void ExecuteCommandLists(const std::vector<CommandList*>& InCommandLists) = 0;

		virtual Shared<CommandAllocator> GetCurrentCommandAllocator() const = 0;
		virtual CommandList* GetCurrentCommandList() const = 0;

		virtual uint32_t GetFramesInFlight() const = 0;
		virtual uint32_t GetCurrentFrame() const = 0;

		virtual void Release() = 0;

	public:
		static Unique<Renderer> Create(const RendererInfo& InInfo);
	};

}
