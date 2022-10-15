#pragma once

#include "Fusion/Core/Window.h"

namespace Fusion {

	class GraphicsContext
	{
	public:
		virtual void ClearBackBuffer(const glm::vec4& InColor) = 0;
		virtual void Present() = 0;

		virtual void OnResize(uint32_t InWidth, uint32_t InHeight) = 0;
	};

}
