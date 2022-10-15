#include "FusionPCH.h"
#include "D3D11Context.h"

namespace Fusion {

	D3D11Context::D3D11Context(const Unique<Window>& InWindow)
	{
		D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &m_Device, NULL, &m_DeviceContext);

		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
		SwapChainDesc.BufferDesc.Width = InWindow->GetWidth();
		SwapChainDesc.BufferDesc.Height = InWindow->GetHeight();
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.SampleDesc.Quality = 0;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = 1;
		SwapChainDesc.OutputWindow = static_cast<HWND>(InWindow->GetWindowHandle());
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		SwapChainDesc.Windowed = true;

		IDXGIDevice* DXGIDevice = NULL;
		IDXGIAdapter* DXGIAdapter = NULL;
		IDXGIFactory* DXGIFactory = NULL;
		m_Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&DXGIDevice));
		DXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&DXGIAdapter));
		DXGIAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&DXGIFactory));
		DXGIFactory->CreateSwapChain(m_Device, &SwapChainDesc, &m_DXGISwapChain);

		FUSION_RELEASE_COM(DXGIFactory);
		FUSION_RELEASE_COM(DXGIAdapter);
		FUSION_RELEASE_COM(DXGIDevice);

		OnResize(InWindow->GetWidth(), InWindow->GetHeight());
	}

	D3D11Context::~D3D11Context()
	{
		m_DXGISwapChain->SetFullscreenState(FALSE, NULL);

		FUSION_RELEASE_COM(m_DXGISwapChain);
		FUSION_RELEASE_COM(m_BackBufferView);
		FUSION_RELEASE_COM(m_Device);
		FUSION_RELEASE_COM(m_DeviceContext);
	}

	void D3D11Context::ClearBackBuffer(const glm::vec4& InColor)
	{
		// NOTE(Peter): ImGui changes the render targets while rendering, so we have to set it back every frame
		m_DeviceContext->OMSetRenderTargets(1, &m_BackBufferView, NULL);
		m_DeviceContext->ClearRenderTargetView(m_BackBufferView, glm::value_ptr(InColor));
	}

	void D3D11Context::Present()
	{
		m_DXGISwapChain->Present(1, 0);
	}

	void D3D11Context::OnResize(uint32_t InWidth, uint32_t InHeight)
	{
		FUSION_RELEASE_COM(m_BackBufferView);

		m_DXGISwapChain->ResizeBuffers(1, InWidth, InHeight, DXGI_FORMAT_B8G8R8A8_UNORM, 0);

		CreateBackBufferView();

		D3D11_VIEWPORT Viewport;
		ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));
		Viewport.TopLeftX = 0;
		Viewport.TopLeftY = 0;
		Viewport.Width = static_cast<float>(InWidth);
		Viewport.Height = static_cast<float>(InHeight);
		m_DeviceContext->RSSetViewports(1, &Viewport);
	}

	void D3D11Context::CreateBackBufferView()
	{
		ID3D11Texture2D* BackBufferTexture;
		m_DXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&BackBufferTexture));
		m_Device->CreateRenderTargetView(BackBufferTexture, NULL, &m_BackBufferView);
		FUSION_RELEASE_COM(BackBufferTexture);
		m_DeviceContext->OMSetRenderTargets(1, &m_BackBufferView, NULL);
	}

}
