#include "FusionPCH.hpp"
#include "D3D11Context.hpp"

#include "Fusion/Core/Application.hpp"

namespace Fusion {

	D3D11Context::D3D11Context()
	{
		s_CurrentContext = this;

		uint32_t DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef FUSION_DEBUG
		DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, DeviceFlags, nullptr, 0, D3D11_SDK_VERSION, m_Device, nullptr, m_DeviceContext);

		CommandAllocatorInfo AllocatorInfo = {};
		AllocatorInfo.ListType = ECommandListType::Direct;
		AllocatorInfo.InitialListCount = 1;
		m_Allocator = Shared<D3D11CommandAllocator>::Create(m_DeviceContext, AllocatorInfo);

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

}
