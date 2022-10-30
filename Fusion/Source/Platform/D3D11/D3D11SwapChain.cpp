#include "FusionPCH.hpp"
#include "D3D11SwapChain.hpp"
#include "D3D11Context.hpp"
#include "Fusion/Core/Application.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Fusion {

	D3D11SwapChain::D3D11SwapChain(const SwapChainInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		Shared<D3D11Context> D3DContext = GraphicsContext::Get<D3D11Context>();
		const auto& TargetWindow = Application::Get().GetWindow();

		uint32_t Width = InCreateInfo.Width == 0 ? TargetWindow->GetWidth() : InCreateInfo.Width;
		uint32_t Height = InCreateInfo.Height == 0 ? TargetWindow->GetHeight() : InCreateInfo.Height;

		m_CreateInfo.Width = Width;
		m_CreateInfo.Height = Height;

		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
		SwapChainDesc.BufferDesc.Width = Width;
		SwapChainDesc.BufferDesc.Height = Height;
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.SampleDesc.Quality = 0;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = 2;
		SwapChainDesc.OutputWindow = glfwGetWin32Window(static_cast<GLFWwindow*>(TargetWindow->GetWindowHandle()));
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		SwapChainDesc.Windowed = true;

		IDXGIDevice* DXGIDevice = NULL;
		IDXGIAdapter* DXGIAdapter = NULL;
		IDXGIFactory* DXGIFactory = NULL;
		D3DContext->GetDevice()->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&DXGIDevice));
		DXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&DXGIAdapter));
		DXGIAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&DXGIFactory));
		DXGIFactory->CreateSwapChain(D3DContext->GetDevice(), &SwapChainDesc, &m_SwapChain);

		FUSION_RELEASE_COM(DXGIFactory);
		FUSION_RELEASE_COM(DXGIAdapter);
		FUSION_RELEASE_COM(DXGIDevice);

		Invalidate();
	}

	D3D11SwapChain::~D3D11SwapChain()
	{
		m_SwapChain->SetFullscreenState(FALSE, NULL);

		FUSION_RELEASE_COM(m_DepthStencilView);
		FUSION_RELEASE_COM(m_DepthStencilBuffer);
		FUSION_RELEASE_COM(m_RenderTargetView);
		FUSION_RELEASE_COM(m_SwapChain);
	}

	void D3D11SwapChain::Bind()
	{
		ID3D11DeviceContext* DeviceContext = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();
		DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
		DeviceContext->RSSetViewports(1, &m_Viewport);
	}

	void D3D11SwapChain::Clear() const
	{
		ID3D11DeviceContext* DeviceContext = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();

		DeviceContext->ClearRenderTargetView(m_RenderTargetView, glm::value_ptr(m_CreateInfo.RenderTargetClearColor));

		if (m_DepthStencilView)
			DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void D3D11SwapChain::Present() const
	{
		m_SwapChain->Present(1, 0);
	}

	void D3D11SwapChain::Resize(uint32_t InWidth, uint32_t InHeight)
	{
		if (InWidth == 0 || InHeight == 0)
			return;

		m_CreateInfo.Width = InWidth;
		m_CreateInfo.Height = InHeight;

		Invalidate();
	}

	void D3D11SwapChain::Invalidate()
	{
		Shared<D3D11Context> D3DContext = GraphicsContext::Get<D3D11Context>();

		ID3D11RenderTargetView* NullViews[] = { nullptr };
		D3DContext->GetDeviceContext()->OMSetRenderTargets(1, NullViews, nullptr);

		FUSION_RELEASE_COM(m_DepthStencilView);
		FUSION_RELEASE_COM(m_DepthStencilBuffer);
		FUSION_RELEASE_COM(m_RenderTargetView);

		m_SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		ID3D11Texture2D* RenderTargetTexture;
		m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&RenderTargetTexture));
		D3DContext->GetDevice()->CreateRenderTargetView(RenderTargetTexture, nullptr, &m_RenderTargetView);
		FUSION_RELEASE_COM(RenderTargetTexture);

		if (m_CreateInfo.HasDepthBuffer)
		{
			D3D11_TEXTURE2D_DESC DepthStencilDesc;
			ZeroMemory(&DepthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));
			DepthStencilDesc.Width = m_CreateInfo.Width;
			DepthStencilDesc.Height = m_CreateInfo.Height;
			DepthStencilDesc.MipLevels = 1;
			DepthStencilDesc.ArraySize = 1;
			DepthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			DepthStencilDesc.SampleDesc.Count = 1;
			DepthStencilDesc.SampleDesc.Quality = 0;
			DepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
			DepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			DepthStencilDesc.CPUAccessFlags = 0;
			DepthStencilDesc.MiscFlags = 0;
			D3DContext->GetDevice()->CreateTexture2D(&DepthStencilDesc, NULL, &m_DepthStencilBuffer);
			D3DContext->GetDevice()->CreateDepthStencilView(m_DepthStencilBuffer, NULL, &m_DepthStencilView);
		}

		D3DContext->GetDeviceContext()->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

		// TODO(Peter): Rasterizer State should belong to a pipeline (allow states to be shared by multiple pipelines)
		D3D11_RASTERIZER_DESC RasterizerDesc;
		ZeroMemory(&RasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
		RasterizerDesc.AntialiasedLineEnable = false;
		RasterizerDesc.CullMode = D3D11_CULL_NONE;
		RasterizerDesc.DepthBias = 0;
		RasterizerDesc.DepthBiasClamp = 0.0f;
		RasterizerDesc.DepthClipEnable = true;
		RasterizerDesc.FillMode = D3D11_FILL_SOLID;
		RasterizerDesc.FrontCounterClockwise = true;
		RasterizerDesc.MultisampleEnable = false;
		RasterizerDesc.ScissorEnable = false;
		RasterizerDesc.SlopeScaledDepthBias = 0.0f;

		ID3D11RasterizerState* RasterizerState;
		D3DContext->GetDevice()->CreateRasterizerState(&RasterizerDesc, &RasterizerState);
		D3DContext->GetDeviceContext()->RSSetState(RasterizerState);
		RasterizerState->Release();

		ZeroMemory(&m_Viewport, sizeof(D3D11_VIEWPORT));
		m_Viewport.TopLeftX = 0;
		m_Viewport.TopLeftY = 0;
		m_Viewport.Width = static_cast<float>(m_CreateInfo.Width);
		m_Viewport.Height = static_cast<float>(m_CreateInfo.Height);
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;
		D3DContext->GetDeviceContext()->RSSetViewports(1, &m_Viewport);
	}

}
