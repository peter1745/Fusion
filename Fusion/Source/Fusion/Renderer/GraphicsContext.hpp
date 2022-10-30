#pragma once

#include "Fusion/Memory/Shared.hpp"
#include "Fusion/Core/Window.hpp"

#include "SwapChain.hpp"

namespace Fusion {

	class GraphicsContext : public SharedObject
	{
	public:
		virtual ~GraphicsContext() = default;

		template<typename TContext>
		inline static Shared<TContext> Get() { return Shared<GraphicsContext>(s_CurrentContext).As<TContext>(); }

	protected:
		inline static GraphicsContext* s_CurrentContext = nullptr;
	};

}
