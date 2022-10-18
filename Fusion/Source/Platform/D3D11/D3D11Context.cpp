#include "FusionPCH.h"
#include "D3D11Context.h"

#include "Fusion/Core/Application.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Fusion {

	D3D11Context::D3D11Context(const Unique<Window>& InWindow)
	{
		s_CurrentContext = this;

		GLFWwindow* NativeWindow = static_cast<GLFWwindow*>(InWindow->GetWindowHandle());

		uint32_t DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef FUSION_DEBUG
		DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, DeviceFlags, nullptr, 0, D3D11_SDK_VERSION, &m_Device, nullptr, &m_DeviceContext);

		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
		SwapChainDesc.BufferDesc.Width = InWindow->GetWidth();
		SwapChainDesc.BufferDesc.Height = InWindow->GetHeight();
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.SampleDesc.Quality = 0;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = 2;
		SwapChainDesc.OutputWindow = glfwGetWin32Window(NativeWindow);
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
		m_DeviceContext->OMSetRenderTargets(1, &m_BackBufferView, m_DepthStencilView);
		m_DeviceContext->ClearRenderTargetView(m_BackBufferView, glm::value_ptr(InColor));

		if (m_DepthStencilView)
			m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		const auto& Window = Application::Get().GetWindow();

		D3D11_VIEWPORT Viewport;
		ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));
		Viewport.TopLeftX = 0;
		Viewport.TopLeftY = 0;
		Viewport.Width = static_cast<float>(Window->GetWidth());
		Viewport.Height = static_cast<float>(Window->GetHeight());
		Viewport.MinDepth = 0.0f;
		Viewport.MaxDepth = 1.0f;
		m_DeviceContext->RSSetViewports(1, &Viewport);
	}

	void D3D11Context::Present()
	{
		m_DXGISwapChain->Present(1, 0);
	}

	void D3D11Context::OnResize(uint32_t InWidth, uint32_t InHeight)
	{
		ID3D11RenderTargetView* nullViews[] = { nullptr };
		m_DeviceContext->OMSetRenderTargets(1, nullViews, nullptr);

		FUSION_RELEASE_COM(m_BackBufferView);
		FUSION_RELEASE_COM(m_DepthStencilView);
		FUSION_RELEASE_COM(m_DepthStencilBuffer);

		m_DXGISwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		CreateBackBufferView();

		/*D3D11_TEXTURE2D_DESC DepthStencilDesc;
		ZeroMemory(&DepthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));
		DepthStencilDesc.Width = InWidth;
		DepthStencilDesc.Height = InHeight;
		DepthStencilDesc.MipLevels = 1;
		DepthStencilDesc.ArraySize = 1;
		DepthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DepthStencilDesc.SampleDesc.Count = 1;
		DepthStencilDesc.SampleDesc.Quality = 0;
		DepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		DepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		DepthStencilDesc.CPUAccessFlags = 0;
		DepthStencilDesc.MiscFlags = 0;
		m_Device->CreateTexture2D(&DepthStencilDesc, NULL, &m_DepthStencilBuffer);
		m_Device->CreateDepthStencilView(m_DepthStencilBuffer, NULL, &m_DepthStencilView);*/

		m_DeviceContext->OMSetRenderTargets(1, &m_BackBufferView, m_DepthStencilView);

		D3D11_VIEWPORT Viewport;
		ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));
		Viewport.TopLeftX = 0;
		Viewport.TopLeftY = 0;
		Viewport.Width = static_cast<float>(InWidth);
		Viewport.Height = static_cast<float>(InHeight);
		Viewport.MinDepth = 0.0f;
		Viewport.MaxDepth = 1.0f;
		m_DeviceContext->RSSetViewports(1, &Viewport);

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
		m_Device->CreateRasterizerState(&RasterizerDesc, &RasterizerState);
		m_DeviceContext->RSSetState(RasterizerState);
		RasterizerState->Release();
	}

	void D3D11Context::SetRenderTargets(ID3D11RenderTargetView* InRenderTarget, ID3D11DepthStencilView* InDepthStencil)
	{
		m_DeviceContext->OMSetRenderTargets(1, &InRenderTarget, InDepthStencil);
	}

	void D3D11Context::CreateBackBufferView()
	{
		ID3D11Texture2D* BackBufferTexture;
		m_DXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&BackBufferTexture));
		m_Device->CreateRenderTargetView(BackBufferTexture, nullptr, &m_BackBufferView);
		FUSION_RELEASE_COM(BackBufferTexture);
	}

}
