#pragma once

#include "Fusion/Memory/Shared.hpp"
#include "Fusion/Core/Window.hpp"

#include "Device.hpp"
#include "DescriptorHeap.hpp"
#include "CommandAllocator.hpp"
#include "Buffer.hpp"

namespace Fusion {

	class GraphicsContext : public SharedObject
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual Shared<Device> GetDevice() const = 0;
		virtual Shared<DescriptorHeap> GetDescriptorHeap(EDescriptorHeapType InType) const = 0;

	public:
		template <typename TContext>
		inline static Shared<TContext> Get()
		{
			return Shared<GraphicsContext>(s_CurrentContext).As<TContext>();
		}

		static Shared<GraphicsContext> Create();

	protected:
		inline static GraphicsContext* s_CurrentContext = nullptr;
	};

}
