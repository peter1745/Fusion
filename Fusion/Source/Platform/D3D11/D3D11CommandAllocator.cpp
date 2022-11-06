#include "FusionPCH.hpp"
#include "D3D11CommandAllocator.hpp"
#include "D3D11Context.hpp"

namespace Fusion {

	D3D11CommandAllocator::D3D11CommandAllocator(D3DComPtr<ID3D11DeviceContext> InDeviceContext, const CommandAllocatorInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		FUSION_CORE_VERIFY(InCreateInfo.ListType == ECommandListType::Direct, "Currently only supports Direct Command Lists!");
		AllocateCommandLists(InDeviceContext, InCreateInfo.InitialListCount);
	}

	D3D11CommandAllocator::D3D11CommandAllocator(const CommandAllocatorInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
	}

	D3D11CommandAllocator::~D3D11CommandAllocator()
	{
		m_CommandLists.clear();
	}

	CommandList* D3D11CommandAllocator::AllocateCommandList()
	{
		return AllocateCommandList(GraphicsContext::Get<D3D11Context>()->GetDeviceContext());
	}

	CommandList* D3D11CommandAllocator::AllocateCommandList(D3DComPtr<ID3D11DeviceContext> InDeviceContext)
	{
		return m_CommandLists.emplace_back(MakeUnique<D3D11CommandList>(this, InDeviceContext)).get();
	}

	std::vector<CommandList*> D3D11CommandAllocator::AllocateCommandLists(size_t InCount)
	{
		return AllocateCommandLists(GraphicsContext::Get<D3D11Context>()->GetDeviceContext(), InCount);
	}

	std::vector<CommandList*> D3D11CommandAllocator::AllocateCommandLists(D3DComPtr<ID3D11DeviceContext> InDeviceContext, size_t InCount)
	{
		std::vector<CommandList*> Result;
		Result.reserve(InCount);
		for (size_t Idx = 0; Idx < InCount; Idx++)
			Result.push_back(AllocateCommandList(InDeviceContext));
		return Result;
	}

}
