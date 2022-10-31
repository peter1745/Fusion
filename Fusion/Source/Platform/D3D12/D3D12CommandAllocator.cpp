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

	D3D12CommandAllocator::D3D12CommandAllocator(D3D12ComPtr<ID3D12Device9> InDevice, const CommandAllocatorInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		FUSION_CORE_VERIFY(InCreateInfo.ListType == ECommandListType::Direct, "Currently only supports Direct Command Lists!");
		auto D3D12CommandListType = ECommandListTypeToD3D12CommandListType(InCreateInfo.ListType);
		InDevice->CreateCommandAllocator(D3D12CommandListType, m_Allocator, m_Allocator);
		AllocateCommandLists(InDevice, InCreateInfo.InitialListCount);
	}

	D3D12CommandAllocator::D3D12CommandAllocator(const CommandAllocatorInfo& InCreateInfo)
		: D3D12CommandAllocator(GraphicsContext::Get<D3D12Context>()->GetDevice(), InCreateInfo)
	{
	}

	D3D12CommandAllocator::~D3D12CommandAllocator()
	{
		m_CommandLists.clear();
	}

	Shared<CommandList> D3D12CommandAllocator::AllocateCommandList()
	{
		return AllocateCommandList(GraphicsContext::Get<D3D12Context>()->GetDevice());
	}

	Shared<CommandList> D3D12CommandAllocator::AllocateCommandList(D3D12ComPtr<ID3D12Device9> InDevice)
	{
		auto D3D12CommandListType = ECommandListTypeToD3D12CommandListType(m_CreateInfo.ListType);

		D3D12ComPtr<ID3D12GraphicsCommandList6> GraphicsCommandList;
		InDevice->CreateCommandList1(0, D3D12CommandListType, D3D12_COMMAND_LIST_FLAG_NONE, GraphicsCommandList, GraphicsCommandList);

		Shared<D3D12CommandList> Result = Shared<D3D12CommandList>::Create(this, GraphicsCommandList);
		m_CommandLists.push_back(Result);
		return Result;
	}

	std::vector<Shared<CommandList>> D3D12CommandAllocator::AllocateCommandLists(size_t InCount)
	{
		return AllocateCommandLists(GraphicsContext::Get<D3D12Context>()->GetDevice(), InCount);
	}

	std::vector<Shared<CommandList>> D3D12CommandAllocator::AllocateCommandLists(D3D12ComPtr<ID3D12Device9> InDevice, size_t InCount)
	{
		std::vector<Shared<CommandList>> Result;
		Result.reserve(InCount);
		for (size_t Idx = 0; Idx < InCount; Idx++)
			Result.push_back(AllocateCommandList(InDevice));
		return Result;
	}

	void D3D12CommandAllocator::Reset()
	{
		m_Allocator->Reset();
		for (auto CmdList : m_CommandLists)
			CmdList->m_CommandList->Reset(m_Allocator, nullptr);
	}

}
