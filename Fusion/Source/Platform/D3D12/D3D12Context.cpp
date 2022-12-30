#include "FusionPCH.hpp"
#include "D3D12Context.hpp"

namespace Fusion {

	D3D12Context::D3D12Context()
	{
		s_CurrentContext = this;

		uint32_t DXGIFactoryFlags = 0;

#ifdef FUSION_DEBUG
		D3DComPtr<ID3D12Debug> DebugInterface;
		if (SUCCEEDED(D3D12GetDebugInterface(DebugInterface, DebugInterface)))
		{
			DebugInterface->EnableDebugLayer();
			DXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif

		CreateDXGIFactory2(DXGIFactoryFlags, m_Factory, m_Factory);

		m_Device = Shared<D3D12Device>::Create(m_Factory);

		if (SUCCEEDED(m_Device->GetDevice()->QueryInterface(m_InfoQueue, m_InfoQueue)))
			m_InfoQueue->RegisterMessageCallback(&MessageCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &m_MessageCallbackCookie);

		// Create Descriptor Heaps
		{
			DescriptorHeapInfo RTVHeapInfo = {};
			RTVHeapInfo.Type = EDescriptorHeapType::RenderTargetView;
			RTVHeapInfo.Capacity = 512;
			RTVHeapInfo.ShaderVisible = false;
			m_DescriptorHeaps[EDescriptorHeapType::RenderTargetView] = Shared<D3D12DescriptorHeap>::Create(m_Device, RTVHeapInfo);

			DescriptorHeapInfo SRV_CBV_UAV_HeapInfo = {};
			SRV_CBV_UAV_HeapInfo.Type = EDescriptorHeapType::SRV_CBV_UAV;
			SRV_CBV_UAV_HeapInfo.Capacity = 1024;
			SRV_CBV_UAV_HeapInfo.ShaderVisible = true;
			m_DescriptorHeaps[EDescriptorHeapType::SRV_CBV_UAV] = Shared<D3D12DescriptorHeap>::Create(m_Device, SRV_CBV_UAV_HeapInfo);
		}
	}

	D3D12Context::~D3D12Context() {}

	/*void D3D12Context::WaitForGPU()
	{
		m_CommandQueue->Signal(m_FrameFence, m_FrameValues[m_FrameIndex]);
		m_FrameFence->SetEventOnCompletion(m_FrameValues[m_FrameIndex], m_FrameEvent);
		WaitForSingleObjectEx(m_FrameEvent, INFINITE, false);
		m_FrameValues[m_FrameIndex]++;
	}*/

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

	void D3D12Context::Release()
	{
	}

	CommandContextQueue::CommandContextQueue()
	{
		m_CommandBuffer = new uint8_t[10 * 1024 * 1024];
		m_CommandBufferPtr = m_CommandBuffer;
		memset(m_CommandBuffer, 0, 10 * 1024 * 1024);
	}

	CommandContextQueue::~CommandContextQueue()
	{
		delete[] m_CommandBuffer;
	}

	void* CommandContextQueue::AllocateCommand(CommandFunc InFunc, uint32_t InSize)
	{
		*reinterpret_cast<CommandFunc*>(m_CommandBufferPtr) = InFunc;
		m_CommandBufferPtr += sizeof(CommandFunc);

		*reinterpret_cast<uint32_t*>(m_CommandBufferPtr) = InSize;
		m_CommandBufferPtr += sizeof(uint32_t);

		void* Memory = m_CommandBufferPtr;
		m_CommandBufferPtr += InSize;

		m_CommandCount++;
		return Memory;
	}

	void CommandContextQueue::Execute(CommandList* InCmdList)
	{
		uint8_t* Buffer = m_CommandBuffer;

		for (uint32_t Idx = 0; Idx < m_CommandCount; Idx++)
		{
			CommandFunc Func = *reinterpret_cast<CommandFunc*>(Buffer);
			Buffer += sizeof(CommandFunc);

			uint32_t Size = *reinterpret_cast<uint32_t*>(Buffer);
			Buffer += sizeof(uint32_t);

			Func(Buffer, InCmdList);
			Buffer += Size;
		}

		m_CommandBufferPtr = m_CommandBuffer;
		m_CommandCount = 0;
	}

}
