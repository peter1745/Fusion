#include "FusionPCH.hpp"
#include "D3D11Renderer.hpp"

namespace Fusion {

	D3D11Renderer::D3D11Renderer(const RendererInfo& InInfo)
	{
		m_Context = GraphicsContext::Get<D3D11Context>();
		m_SwapChain = InInfo.TargetSwapChain.As<D3D11SwapChain>();

		auto Device = m_Context->GetDevice().As<D3D11Device>();

		// Create a command allocator
		{
			CommandAllocatorInfo AllocatorInfo = {};
			AllocatorInfo.ListType = ECommandListType::Direct;
			AllocatorInfo.InitialListCount = 1;
			m_CommandAllocator = Shared<D3D11CommandAllocator>::Create(Device, AllocatorInfo);
		}
	}

	void D3D11Renderer::BeginFrame()
	{
		m_CommandAllocator->Reset();
		m_CommandAllocator->GetCommandList(0)->BeginRecording();
	}

	void D3D11Renderer::EndFrame()
	{
		m_CommandAllocator->GetCommandList(0)->EndRecording();

		m_SwapChain->Present();
	}

	void D3D11Renderer::ExecuteCommandLists(const std::vector<CommandList*>& InCommandLists)
	{
	}

	void D3D11Renderer::SubmitResourceForDestruction(uint32_t InFrameOffset, const std::function<void()>& InFunc)
	{
	}

	void D3D11Renderer::Release()
	{
		m_CommandAllocator->Release();
		m_CommandAllocator = nullptr;
	}

}
