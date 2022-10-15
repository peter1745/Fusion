#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "CommandBuffer.h"
#include "GraphicsContext.h"

namespace Fusion {

	enum class ERendererAPI { None = -1, D3D11 };

	class Renderer : public SharedObject
	{
	public:
		virtual ~Renderer() = default;

		virtual void Begin() = 0;
		virtual void DrawIndexed(const Shared<VertexBuffer>& InVertexBuffer, const Shared<IndexBuffer>& InIndexBuffer, const Shared<Shader>& InShader) = 0;
		virtual void Clear() = 0;
		virtual void End() = 0;

		virtual void OnResize(uint32_t InWidth, uint32_t InHeight) = 0;

		virtual void Present() = 0;

		virtual Shared<GraphicsContext> GetContext() const = 0;

		inline static ERendererAPI CurrentAPI() { return s_CurrentAPI; }

	private:
		static Shared<Renderer> Create(ERendererAPI InRendererAPI);
		inline static ERendererAPI s_CurrentAPI = ERendererAPI::None;

		friend class Application;
	};

}
