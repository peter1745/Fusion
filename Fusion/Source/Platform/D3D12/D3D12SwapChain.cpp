#include "FusionPCH.hpp"
#include "D3D12SwapChain.hpp"

#include "D3D12Context.hpp"

#include "Fusion/Core/Application.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Fusion {

	D3D12SwapChain::D3D12SwapChain(const Shared<GraphicsContext>& InContext, const SwapChainInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		m_Device = InContext->GetDevice().As<D3D12Device>();

		DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
		SwapChainDesc.BufferCount = 3;
		SwapChainDesc.Width = InCreateInfo.Width;
		SwapChainDesc.Height = InCreateInfo.Height;
		SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

		HWND WindowHandle = glfwGetWin32Window(static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetWindowHandle()));
		D3DComPtr<IDXGISwapChain1> SwapChain;
		InContext.As<D3D12Context>()->GetDXGIFactory()->CreateSwapChainForHwnd(m_Device->GetGraphicsCommandQueue(), WindowHandle, &SwapChainDesc, nullptr, nullptr, SwapChain);
		InContext.As<D3D12Context>()->GetDXGIFactory()->MakeWindowAssociation(WindowHandle, DXGI_MWA_NO_ALT_ENTER);

		m_SwapChain = SwapChain;
		m_SwapChain->GetDesc1(&SwapChainDesc);

		m_CurrentImageIndex = m_SwapChain->GetCurrentBackBufferIndex();

		m_Images.resize(SwapChainDesc.BufferCount);

		// TODO(Peter): Move Descriptor heaps elsewhere
		D3D12_DESCRIPTOR_HEAP_DESC RTVDescriptorHeapDesc = {};
		RTVDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		RTVDescriptorHeapDesc.NumDescriptors = SwapChainDesc.BufferCount;
		RTVDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		RTVDescriptorHeapDesc.NodeMask = 0;
		m_Device->GetDevice()->CreateDescriptorHeap(&RTVDescriptorHeapDesc, m_RTVDescriptorHeap, m_RTVDescriptorHeap);

		m_RTVHeapStart = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_RTVHeapIncrement = m_Device->GetDevice()->GetDescriptorHandleIncrementSize(RTVDescriptorHeapDesc.Type);

		auto Handle = m_RTVHeapStart;
		for (uint32_t Idx = 0; Idx < SwapChainDesc.BufferCount; Idx++)
		{
			m_SwapChain->GetBuffer(Idx, m_Images[Idx], m_Images[Idx]);
			m_Device->GetDevice()->CreateRenderTargetView(m_Images[Idx], nullptr, Handle);
			Handle.ptr += m_RTVHeapIncrement;
		}
	}

	D3D12SwapChain::~D3D12SwapChain()
	{
	}

	void D3D12SwapChain::Bind(CommandList* InCommandList)
	{
		auto& CmdList = static_cast<D3D12CommandList*>(InCommandList)->GetNativeList();

		D3D12_VIEWPORT Viewport = {};
		Viewport.TopLeftX = 0.0f;
		Viewport.TopLeftY = 0.0f;
		Viewport.Width = float(m_CreateInfo.Width);
		Viewport.Height = float(m_CreateInfo.Height);
		Viewport.MinDepth = 0.0f;
		Viewport.MaxDepth = 1.0f;

		D3D12_RECT ScissorRect = {};
		ScissorRect.left = 0;
		ScissorRect.top = 0;
		ScissorRect.right = Viewport.Width;
		ScissorRect.bottom = Viewport.Height;

		CmdList->RSSetViewports(1, &Viewport);
		CmdList->RSSetScissorRects(1, &ScissorRect);

		// Transition image from present to rendering mode
		D3D12_RESOURCE_BARRIER Barrier = {};
		Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		Barrier.Transition.pResource = m_Images[m_CurrentImageIndex];
		Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		Barrier.Transition.Subresource = 0;
		CmdList->ResourceBarrier(1, &Barrier);

		D3D12_CPU_DESCRIPTOR_HANDLE CurrentImageHandle = m_RTVHeapStart;
		CurrentImageHandle.ptr += m_CurrentImageIndex * m_RTVHeapIncrement;
		CmdList->OMSetRenderTargets(1, &CurrentImageHandle, false, nullptr);
	}

	void D3D12SwapChain::Clear(CommandList* InCommandList)
	{
		auto& CmdList = static_cast<D3D12CommandList*>(InCommandList)->GetNativeList();

		D3D12_CPU_DESCRIPTOR_HANDLE CurrentImageHandle = m_RTVHeapStart;
		CurrentImageHandle.ptr += m_CurrentImageIndex * m_RTVHeapIncrement;
		CmdList->ClearRenderTargetView(CurrentImageHandle, glm::value_ptr(m_CreateInfo.RenderTargetClearColor), 0, nullptr);
	}

	void D3D12SwapChain::Present(D3DComPtr<ID3D12Fence1> InFence, HANDLE InEvent, uint64_t InFenceValue)
	{
		m_SwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);

		const auto& Window = Application::Get().GetWindow();
		if (Window->GetWidth() != m_CreateInfo.Width || Window->GetHeight() != m_CreateInfo.Height)
		{
			m_Device->Wait(InFence, InEvent, InFenceValue);
			Resize(Window->GetWidth(), Window->GetHeight());
		}

		m_CurrentImageIndex = m_SwapChain->GetCurrentBackBufferIndex();
	}

	void D3D12SwapChain::Unbind(CommandList* InCommandList)
	{
		auto& CmdList = static_cast<D3D12CommandList*>(InCommandList)->GetNativeList();

		// Transition image from rendering to present mode
		D3D12_RESOURCE_BARRIER Barrier = {};
		Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		Barrier.Transition.pResource = m_Images[m_CurrentImageIndex];
		Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		Barrier.Transition.Subresource = 0;
		CmdList->ResourceBarrier(1, &Barrier);
	}

	void D3D12SwapChain::Resize(uint32_t InWidth, uint32_t InHeight)
	{
		if (InWidth == 0 || InHeight == 0)
			return;

		auto Context = GraphicsContext::Get<D3D12Context>();

		m_Images.clear();

		//m_Device->Wait();

		m_SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);

		DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
		m_SwapChain->GetDesc1(&SwapChainDesc);
		m_Images.resize(SwapChainDesc.BufferCount);

		auto Handle = m_RTVHeapStart;
		for (uint32_t Idx = 0; Idx < SwapChainDesc.BufferCount; Idx++)
		{
			m_SwapChain->GetBuffer(Idx, m_Images[Idx], m_Images[Idx]);
			m_Device->GetDevice()->CreateRenderTargetView(m_Images[Idx], nullptr, Handle);
			Handle.ptr += m_RTVHeapIncrement;
		}

		m_CreateInfo.Width = InWidth;
		m_CreateInfo.Height = InHeight;
	}

	void D3D12SwapChain::Release()
	{
		m_RTVDescriptorHeap.Release();
		m_Images.clear();
		m_SwapChain.Release();
	}

}