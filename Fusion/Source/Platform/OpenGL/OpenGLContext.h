#pragma once

#include "Fusion/Renderer/GraphicsContext.h"

namespace Fusion {

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(const Unique<Window>& InWindow);
		~OpenGLContext();

	};

}
