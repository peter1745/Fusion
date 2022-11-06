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

		D3DComPtr<IDXGIDevice> DXGIDevice;
		D3DComPtr<IDXGIAdapter> DXGIAdapter;
		D3DComPtr<IDXGIFactory> DXGIFactory;
		D3DContext->GetDevice()->QueryInterface(DXGIDevice, DXGIDevice);
		DXGIDevice->GetParent(DXGIAdapter, DXGIAdapter);
		DXGIAdapter->GetParent(DXGIFactory, DXGIFactory);
		DXGIFactory->CreateSwapChain(D3DContext->GetDevice(), &SwapChainDesc, m_SwapChain);

		DXGIFactory.Release();
		DXGIAdapter.Release();
		DXGIDevice.Release();

		Invalidate();
	}

	D3D11SwapChain::~D3D11SwapChain()
	{
		m_SwapChain->SetFullscreenState(false, nullptr);
	}

	void D3D11SwapChain::Bind()
	{
		ID3D11DeviceContext* DeviceContext = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();
		DeviceContext->OMSetRenderTargets(1, m_RenderTargetView, nullptr);
		DeviceContext->RSSetViewports(1, &m_Viewport);
	}

	void D3D11SwapChain::Clear()
	{
		ID3D11DeviceContext* DeviceContext = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();

		DeviceContext->ClearRenderTargetView(m_RenderTargetView, glm::value_ptr(m_CreateInfo.RenderTargetClearColor));
	}

	void D3D11SwapChain::Present()
	{
		m_SwapChain->Present(1, 0);

		const auto& Window = Application::Get().GetWindow();
		if (Window->GetWidth() != m_CreateInfo.Width || Window->GetHeight() != m_CreateInfo.Height)
		{
			Resize(Window->GetWidth(), Window->GetHeight());
		}
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

		m_RenderTargetView.Release();

		m_SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		D3DComPtr<ID3D11Texture2D> RenderTargetTexture;
		m_SwapChain->GetBuffer(0, RenderTargetTexture, RenderTargetTexture);
		D3DContext->GetDevice()->CreateRenderTargetView(RenderTargetTexture, nullptr, m_RenderTargetView);
		RenderTargetTexture.Release();

		D3DContext->GetDeviceContext()->OMSetRenderTargets(1, m_RenderTargetView, nullptr);

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
