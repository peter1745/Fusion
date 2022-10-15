#include "FusionPCH.h"
#include "D3D11Renderer.h"

#include "D3D11Context.h"

#include "Fusion/Core/Application.h"

namespace Fusion {

	struct D3D11RenderStorage
	{
		Shared<D3D11Context> Context = nullptr;
	};

	static D3D11RenderStorage* s_Storage = nullptr;

	D3D11Renderer::D3D11Renderer()
	{
		FUSION_CORE_VERIFY(!s_Storage);
		s_Storage = new D3D11RenderStorage();
		s_Storage->Context = Shared<D3D11Context>::Create(Application::Get().GetWindow());
	}

	D3D11Renderer::~D3D11Renderer()
	{
		delete s_Storage;
	}

	void D3D11Renderer::Begin()
	{
		s_Storage->Context->ClearBackBuffer({ 1.0f, 0.0f, 0.0f, 1.0f });
	}

	void D3D11Renderer::DrawIndexed(const Shared<VertexBuffer>& InVertexBuffer, const Shared<IndexBuffer>& InIndexBuffer, const Shared<Shader>& InShader)
	{

	}

	void D3D11Renderer::Clear()
	{

	}

	void D3D11Renderer::End()
	{

	}

	void D3D11Renderer::OnResize(uint32_t InWidth, uint32_t InHeight)
	{
		s_Storage->Context->OnResize(InWidth, InHeight);
	}

	void D3D11Renderer::Present()
	{
		s_Storage->Context->Present();
	}

	Shared<GraphicsContext> D3D11Renderer::GetContext() const { return s_Storage->Context; }

}
