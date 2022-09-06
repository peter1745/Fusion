#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"

namespace Fusion {

	class Renderer
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void Begin();
		static void DrawIndexed(const Shared<VertexBuffer>& InVertexBuffer, const Shared<IndexBuffer>& InIndexBuffer, const Shared<Shader>& InShader);
		static void Clear();
		static void End();
	};

}
