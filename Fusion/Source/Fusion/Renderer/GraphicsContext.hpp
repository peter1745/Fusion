#pragma once

#include "Fusion/Memory/Shared.hpp"
#include "Fusion/Core/Window.hpp"

#include "DescriptorHeap.hpp"
#include "CommandAllocator.hpp"
#include "Buffer.hpp"

namespace Fusion {

	class GraphicsContext : public SharedObject
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual Shared<CommandAllocator> GetCommandAllocator() const = 0;
		virtual CommandList* GetCurrentCommandList() const = 0;
		virtual void ExecuteCommandLists(const std::vector<CommandList*>& InCommandLists) = 0;

		virtual void NextFrame() = 0;
		virtual void WaitForGPU() = 0;

		virtual Shared<DescriptorHeap> GetDescriptorHeap(EDescriptorHeapType InType) const = 0;

		virtual uint32_t GetFramesInFlight() const = 0;
		virtual uint32_t GetCurrentFrameIndex() const = 0;

	public:
		template<typename TContext>
		inline static Shared<TContext> Get() { return Shared<GraphicsContext>(s_CurrentContext).As<TContext>(); }

		static Shared<GraphicsContext> Create();

	protected:
		inline static GraphicsContext* s_CurrentContext = nullptr;
	};

}
