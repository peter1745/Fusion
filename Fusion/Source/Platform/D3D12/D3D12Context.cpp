#include "FusionPCH.hpp"
#include "D3D12Context.hpp"

namespace Fusion {

	D3D12Context::D3D12Context()
	{
		s_CurrentContext = this;

		uint32_t DXGIFactoryFlags = 0;

#ifdef FUSION_DEBUG
		D3D12ComPtr<ID3D12Debug> DebugInterface;
		if (SUCCEEDED(D3D12GetDebugInterface(DebugInterface, DebugInterface)))
		{
			DebugInterface->EnableDebugLayer();
			DXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif

		CreateDXGIFactory2(DXGIFactoryFlags, m_Factory, m_Factory);

		D3D12ComPtr<IDXGIAdapter1> DXGIAdapter;
		for (UINT AdapterIdx = 0; SUCCEEDED(m_Factory->EnumAdapterByGpuPreference(AdapterIdx, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, DXGIAdapter, DXGIAdapter)); AdapterIdx++)
		{
			DXGI_ADAPTER_DESC1 AdapterDesc;
			DXGIAdapter->GetDesc1(&AdapterDesc);

			if (AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;

			if (SUCCEEDED(D3D12CreateDevice(DXGIAdapter, D3D_FEATURE_LEVEL_12_2, __uuidof(ID3D12Device9), nullptr)))
				break;
		}

		if (!DXGIAdapter.IsValid())
		{
			for (UINT AdapterIdx = 0; SUCCEEDED(m_Factory->EnumAdapters1(AdapterIdx, DXGIAdapter)); AdapterIdx++)
			{
				DXGI_ADAPTER_DESC1 AdapterDesc;
				DXGIAdapter->GetDesc1(&AdapterDesc);

				if (AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					continue;

				if (SUCCEEDED(D3D12CreateDevice(DXGIAdapter, D3D_FEATURE_LEVEL_12_2, __uuidof(ID3D12Device9), nullptr)))
					break;
			}
		}

		FUSION_CORE_VERIFY(DXGIAdapter.IsValid(), "Failed to find suitable GPU!");

		D3D12CreateDevice(DXGIAdapter.Get(), D3D_FEATURE_LEVEL_12_2, m_Device, m_Device);

		if (SUCCEEDED(m_Device->QueryInterface(m_InfoQueue, m_InfoQueue)))
			m_InfoQueue->RegisterMessageCallback(&MessageCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &m_MessageCallbackCookie);

		D3D12_COMMAND_QUEUE_DESC CommandQueueDesc = {};
		CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		CommandQueueDesc.NodeMask = 0;
		m_Device->CreateCommandQueue(&CommandQueueDesc, m_CommandQueue, m_CommandQueue);

		m_FrameValues.resize(m_FrameCount, 0);

		for (uint32_t Idx = 0; Idx < m_FrameCount; Idx++)
		{
			CommandAllocatorInfo AllocatorInfo = {};
			AllocatorInfo.ListType = ECommandListType::Direct;
			AllocatorInfo.InitialListCount = 1;
			m_CommandAllocators.push_back(Shared<D3D12CommandAllocator>::Create(m_Device, AllocatorInfo));
		}

		m_Device->CreateFence(m_FrameValues[m_FrameIndex], D3D12_FENCE_FLAG_NONE, m_FrameFence, m_FrameFence);
		m_FrameEvent = CreateEventW(nullptr, false, false, nullptr);
	}

	D3D12Context::~D3D12Context() {}

	void D3D12Context::ExecuteCommandLists(const std::vector<CommandList*>& InCommandLists)
	{
		std::vector<ID3D12CommandList*> CommandLists(InCommandLists.size());
		for (size_t Idx = 0; Idx < InCommandLists.size(); Idx++)
			CommandLists[Idx] = static_cast<D3D12CommandList*>(InCommandLists[Idx])->GetNativeList();
		m_CommandQueue->ExecuteCommandLists(uint32_t(InCommandLists.size()), CommandLists.data());
	}

	void D3D12Context::NextFrame()
	{
		m_FramesBeforeWait++;
		m_FrameIndex = (m_FrameIndex + 1) % m_FrameCount;

		uint64_t CurrentValue = m_FrameValues[m_FrameIndex];
		m_CommandQueue->Signal(m_FrameFence, CurrentValue);

		if (m_FrameFence->GetCompletedValue() < m_FrameValues[m_FrameIndex])
		{
			//FUSION_CORE_INFO("Rendered {} frames before waiting", m_FramesBeforeWait);
			m_FramesBeforeWait = 0;
			m_FrameFence->SetEventOnCompletion(m_FrameValues[m_FrameIndex], m_FrameEvent);
			WaitForSingleObjectEx(m_FrameEvent, INFINITE, false);
		}

		m_FrameValues[m_FrameIndex] = CurrentValue + 1;
		m_CommandAllocators[m_FrameIndex]->Reset();
	}

	void D3D12Context::WaitForGPU()
	{
		m_CommandQueue->Signal(m_FrameFence, m_FrameValues[m_FrameIndex]);
		m_FrameFence->SetEventOnCompletion(m_FrameValues[m_FrameIndex], m_FrameEvent);
		WaitForSingleObjectEx(m_FrameEvent, INFINITE, false);
		m_FrameValues[m_FrameIndex]++;
	}

	void D3D12Context::MessageCallback(D3D12_MESSAGE_CATEGORY InCategory, D3D12_MESSAGE_SEVERITY InSeverity, D3D12_MESSAGE_ID InID, LPCSTR InDescription, void* InContext)
	{
		switch (InSeverity)
		{
		case D3D12_MESSAGE_SEVERITY_MESSAGE:
			FUSION_CORE_INFO("{}", InDescription);
			break;
		case D3D12_MESSAGE_SEVERITY_INFO:
			FUSION_CORE_INFO("{}", InDescription);
			break;
		case D3D12_MESSAGE_SEVERITY_WARNING:
			FUSION_CORE_WARN("{}", InDescription);
			break;
		case D3D12_MESSAGE_SEVERITY_ERROR:
			FUSION_CORE_ERROR("{}", InDescription);
			break;
		case D3D12_MESSAGE_SEVERITY_CORRUPTION:
			FUSION_CORE_FATAL("{}", InDescription);
			break;
		}
	}

}
