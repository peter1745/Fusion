#include "FusionPCH.hpp"
#include "D3D12Renderer.hpp"

namespace Fusion {

	D3D12Renderer::D3D12Renderer(const RendererInfo& InInfo)
	{
		m_Context = GraphicsContext::Get<D3D12Context>();
		m_SwapChain = InInfo.TargetSwapChain.As<D3D12SwapChain>();

		m_FramesInFlight = std::min(m_FramesInFlight, m_SwapChain->GetImageCount());

		auto Device = m_Context->GetDevice().As<D3D12Device>();

		// Create frame fence
		{
			m_FrameValues.resize(m_FramesInFlight, 0);
			m_FrameFences.resize(m_FramesInFlight);

			for (uint32_t FrameIdx = 0; FrameIdx < m_FramesInFlight; FrameIdx++)
				Device->GetDevice()->CreateFence(m_FrameValues[FrameIdx], D3D12_FENCE_FLAG_NONE, m_FrameFences[FrameIdx], m_FrameFences[FrameIdx]);

			m_FrameEvent = CreateEventW(nullptr, false, false, nullptr);
		}

		// Create Command Allocators
		CommandAllocatorInfo AllocatorInfo = {};
		AllocatorInfo.ListType = ECommandListType::Direct;
		AllocatorInfo.InitialListCount = 1;
		for (uint32_t Idx = 0; Idx < m_FramesInFlight; Idx++)
			m_CommandAllocators.push_back(Shared<D3D12CommandAllocator>::Create(Device, AllocatorInfo));
	}

	void D3D12Renderer::BeginFrame()
	{
		auto Device = m_Context->GetDevice().As<D3D12Device>();

		uint64_t CurrentValue = m_FrameValues[m_CurrentFrame];
		Device->GetGraphicsCommandQueue()->Signal(m_FrameFences[m_CurrentFrame], CurrentValue);

		if (m_FrameFences[m_CurrentFrame]->GetCompletedValue() < m_FrameValues[m_CurrentFrame])
		{
			m_FrameFences[m_CurrentFrame]->SetEventOnCompletion(m_FrameValues[m_CurrentFrame], m_FrameEvent);
			WaitForSingleObjectEx(m_FrameEvent, INFINITE, false);
		}

		m_FrameValues[m_CurrentFrame] = CurrentValue + 1;
		m_CommandAllocators[m_CurrentFrame]->Reset();
		//m_CommandContextQueue.Execute(m_CommandAllocators[m_FrameIndex]->GetCommandList(0)); TODO?
	}

	void D3D12Renderer::EndFrame()
	{
		auto Device = m_Context->GetDevice().As<D3D12Device>();

		auto* CommandList = dynamic_cast<D3D12CommandList*>(m_CommandAllocators[m_CurrentFrame]->GetCommandList(0));
		CommandList->EndRecording();

		std::array<ID3D12CommandList*, 1> List = { CommandList->GetNativeList() };
		Device->GetGraphicsCommandQueue()->ExecuteCommandLists(1, List.data());

		m_SwapChain->Present(m_FrameFences[m_CurrentFrame], m_FrameEvent, m_FrameValues[m_CurrentFrame]);

		m_CurrentFrame = (m_CurrentFrame + 1) % m_FramesInFlight;
	}

	void D3D12Renderer::ExecuteCommandLists(const std::vector<CommandList*>& InCommandLists)
	{
		auto Device = m_Context->GetDevice().As<D3D12Device>();
		std::vector<ID3D12CommandList*> CommandLists(InCommandLists.size());
		for (size_t Idx = 0; Idx < InCommandLists.size(); Idx++)
			CommandLists[Idx] = dynamic_cast<D3D12CommandList*>(InCommandLists[Idx])->GetNativeList();
		Device->GetGraphicsCommandQueue()->ExecuteCommandLists(uint32_t(InCommandLists.size()), CommandLists.data());
	}

	void D3D12Renderer::SubmitResourceForDestruction(uint32_t InFrameOffset, const std::function<void()>& InFunc)
	{
	}

	void D3D12Renderer::Release()
	{
	}
}