#pragma once

#include "Fusion/Renderer/Renderer.hpp"

namespace Fusion {

	class D3D11Renderer : public Renderer
	{
	public:
		D3D11Renderer();
		~D3D11Renderer();

		virtual void Begin() override;
		virtual void DrawIndexed(const Shared<VertexBuffer>& InVertexBuffer, const Shared<IndexBuffer>& InIndexBuffer, const Shared<Shader>& InShader) override;
		virtual void Clear() override;
		virtual void End() override;

		virtual void OnResize(uint32_t InWidth, uint32_t InHeight) override;

		virtual void Present() override;

		virtual Shared<GraphicsContext> GetContext() const override;
	};

}
