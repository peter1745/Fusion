#pragma once

#include "Fusion/Memory/Shared.hpp"
#include "Fusion/Core/Window.hpp"

namespace Fusion {

	class GraphicsContext : public SharedObject
	{
	public:
		virtual void ClearBackBuffer(const glm::vec4& InColor) = 0;
		virtual void Present() = 0;

		virtual void OnResize(uint32_t InWidth, uint32_t InHeight) = 0;

		template<typename TContext>
		inline static Shared<TContext> Get() { return Shared<GraphicsContext>(s_CurrentContext).As<TContext>(); }

	protected:
		inline static GraphicsContext* s_CurrentContext = nullptr;
	};

}