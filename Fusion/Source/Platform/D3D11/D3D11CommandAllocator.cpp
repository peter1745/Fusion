#include "FusionPCH.hpp"
#include "D3D11CommandAllocator.hpp"
#include "D3D11Context.hpp"

namespace Fusion {

	D3D11CommandAllocator::D3D11CommandAllocator(const Shared<D3D11Device>& InDevice, const CommandAllocatorInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo), m_Device(InDevice)
	{
		for (uint32_t Idx = 0; Idx < m_CreateInfo.InitialListCount; Idx++)
			m_CommandLists.emplace_back(MakeUnique<D3D11CommandList>(this, m_Device->GetDeviceContext()));
	}

	D3D11CommandAllocator::~D3D11CommandAllocator()
	{
		m_CommandLists.clear();
		m_Device.Reset();
	}

	CommandList* D3D11CommandAllocator::AllocateCommandList()
	{
		return m_CommandLists.emplace_back(MakeUnique<D3D11CommandList>(this, m_Device->GetDeviceContext())).get();
	}

	std::vector<CommandList*> D3D11CommandAllocator::AllocateCommandLists(size_t InCount)
	{
		std::vector<CommandList*> Result;
		Result.reserve(InCount);
		for (size_t Idx = 0; Idx < InCount; Idx++)
			Result.push_back(AllocateCommandList());
		return Result;
	}

	void D3D11CommandAllocator::DestroyCommandList(CommandList* InCommandList)
	{
		FUSION_CORE_VERIFY(false);
	}

	void D3D11CommandAllocator::Release()
	{
		m_CommandLists.clear();
		m_Device.Reset();
	}

}
