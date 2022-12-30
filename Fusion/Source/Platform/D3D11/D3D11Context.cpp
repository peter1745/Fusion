#include "FusionPCH.hpp"
#include "D3D11Context.hpp"

#include "Fusion/Core/Application.hpp"

namespace Fusion {

	D3D11Context::D3D11Context()
	{
		s_CurrentContext = this;

		m_Device = Shared<D3D11Device>::Create();

		CommandAllocatorInfo AllocatorInfo = {};
		AllocatorInfo.ListType = ECommandListType::Direct;
		AllocatorInfo.InitialListCount = 1;
		m_Allocator = Shared<D3D11CommandAllocator>::Create(m_Device, AllocatorInfo);

		// Create Descriptor Heaps
		{
			DescriptorHeapInfo RTVHeapInfo = {};
			RTVHeapInfo.Type = EDescriptorHeapType::RenderTargetView;
			RTVHeapInfo.Capacity = 512;
			RTVHeapInfo.ShaderVisible = false;
			m_DescriptorHeaps[EDescriptorHeapType::RenderTargetView] = Shared<D3D11DescriptorHeap>::Create(RTVHeapInfo);

			DescriptorHeapInfo SRV_CBV_UAV_HeapInfo = {};
			SRV_CBV_UAV_HeapInfo.Type = EDescriptorHeapType::SRV_CBV_UAV;
			SRV_CBV_UAV_HeapInfo.Capacity = 1024;
			SRV_CBV_UAV_HeapInfo.ShaderVisible = true;
			m_DescriptorHeaps[EDescriptorHeapType::SRV_CBV_UAV] = Shared<D3D11DescriptorHeap>::Create(SRV_CBV_UAV_HeapInfo);
		}
	}

	D3D11Context::~D3D11Context()
	{
	}

	void D3D11Context::Release()
	{
	}

}
