#include "FusionPCH.hpp"
#include "D3D12CommandAllocator.hpp"

#include "D3D12Context.hpp"

namespace Fusion {

	static D3D12_COMMAND_LIST_TYPE ECommandListTypeToD3D12CommandListType(ECommandListType InType)
	{
		switch (InType)
		{
		case ECommandListType::Direct:  return D3D12_COMMAND_LIST_TYPE_DIRECT;
		case ECommandListType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
		}

		return D3D12_COMMAND_LIST_TYPE_DIRECT;
	}

	D3D12CommandAllocator::D3D12CommandAllocator(const Shared<D3D12Device>& InDevice, const CommandAllocatorInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo), m_Device(InDevice)
	{
		FUSION_CORE_VERIFY(InCreateInfo.ListType == ECommandListType::Direct, "Currently only supports Direct Command Lists!");
		auto D3D12CommandListType = ECommandListTypeToD3D12CommandListType(InCreateInfo.ListType);
		m_Device->GetDevice()->CreateCommandAllocator(D3D12CommandListType, m_Allocator, m_Allocator);

		for (uint32_t Idx = 0; Idx < InCreateInfo.InitialListCount; Idx++)
		{
			D3DComPtr<ID3D12GraphicsCommandList6> GraphicsCommandList;
			m_Device->GetDevice()->CreateCommandList1(0, D3D12CommandListType, D3D12_COMMAND_LIST_FLAG_NONE, GraphicsCommandList, GraphicsCommandList);
			m_CommandLists.emplace_back(MakeUnique<D3D12CommandList>(this, GraphicsCommandList));
		}
	}

	D3D12CommandAllocator::~D3D12CommandAllocator()
	{
	}

	CommandList* D3D12CommandAllocator::AllocateCommandList()
	{
		auto D3D12CommandListType = ECommandListTypeToD3D12CommandListType(m_CreateInfo.ListType);

		D3DComPtr<ID3D12GraphicsCommandList6> GraphicsCommandList;
		m_Device->GetDevice()->CreateCommandList1(0, D3D12CommandListType, D3D12_COMMAND_LIST_FLAG_NONE, GraphicsCommandList, GraphicsCommandList);
		return m_CommandLists.emplace_back(MakeUnique<D3D12CommandList>(this, GraphicsCommandList)).get();
	}

	std::vector<CommandList*> D3D12CommandAllocator::AllocateCommandLists(size_t InCount)
	{
		std::vector<CommandList*> Result;
		Result.reserve(InCount);
		for (size_t Idx = 0; Idx < InCount; Idx++)
			Result.push_back(AllocateCommandList());
		return Result;
	}

	void D3D12CommandAllocator::DestroyCommandList(CommandList* InCommandList)
	{
		FUSION_CORE_VERIFY(false);
	}

	void D3D12CommandAllocator::Reset()
	{
		m_Allocator->Reset();
		for (auto& CmdList : m_CommandLists)
			CmdList->m_CommandList->Reset(m_Allocator, nullptr);
	}

	void D3D12CommandAllocator::Release()
	{
		m_CommandLists.clear();
		m_Allocator.Release();
		m_Device.Reset();
	}

}
