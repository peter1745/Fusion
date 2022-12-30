#include "FusionPCH.hpp"
#include "D3D12Device.hpp"

namespace Fusion {

	D3D12Device::D3D12Device(D3DComPtr<IDXGIFactory7> InFactory)
	{
		// TODO(Peter): Use a scoring system just like Vulkan (preferably standardize it)
		D3DComPtr<IDXGIAdapter1> DXGIAdapter;
		for (UINT AdapterIdx = 0; SUCCEEDED(InFactory->EnumAdapterByGpuPreference(AdapterIdx, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, DXGIAdapter, DXGIAdapter)); AdapterIdx++)
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
			for (UINT AdapterIdx = 0; SUCCEEDED(InFactory->EnumAdapters1(AdapterIdx, DXGIAdapter)); AdapterIdx++)
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

		// Create Graphics Command Queue
		D3D12_COMMAND_QUEUE_DESC CommandQueueDesc = {};
		CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		CommandQueueDesc.NodeMask = 0;
		m_Device->CreateCommandQueue(&CommandQueueDesc, m_GraphicsCommandQueue, m_GraphicsCommandQueue);
	}

	void D3D12Device::Wait() const
	{
	}

	void D3D12Device::Wait(D3DComPtr<ID3D12Fence1> InFence, HANDLE InEvent, uint64_t InFenceValue)
	{
		m_GraphicsCommandQueue->Signal(InFence, InFenceValue);

		if (InFence->GetCompletedValue() < InFenceValue)
		{
			InFence->SetEventOnCompletion(InFenceValue, InEvent);
			WaitForSingleObjectEx(InEvent, INFINITE, false);
		}
	}

	void D3D12Device::Release()
	{
		m_Device.Release();
	}
}