#pragma once

#include "Fusion/Renderer/CommandAllocator.hpp"
#include "D3D12CommandList.hpp"

namespace Fusion {

	class D3D12CommandAllocator : public CommandAllocator
	{
	public:
		D3D12CommandAllocator(D3DComPtr<ID3D12Device9> InDevice, const CommandAllocatorInfo& InCreateInfo);
		D3D12CommandAllocator(const CommandAllocatorInfo& InCreateInfo);
		~D3D12CommandAllocator();

		virtual CommandList* AllocateCommandList() override;
		virtual std::vector<CommandList*> AllocateCommandLists(size_t InCount) override;

		virtual CommandList* GetCommandList(size_t InIndex) const override
		{
			FUSION_CORE_VERIFY(InIndex < m_CommandLists.size());
			return m_CommandLists[InIndex].get();
		}

		virtual void Reset() override;

		auto& GetNativeAllocator() { return m_Allocator; }

	private:
		CommandList* AllocateCommandList(D3DComPtr<ID3D12Device9> InDevice);
		std::vector<CommandList*> AllocateCommandLists(D3DComPtr<ID3D12Device9> InDevice, size_t InCount);

	private:
		CommandAllocatorInfo m_CreateInfo;
		D3DComPtr<ID3D12CommandAllocator> m_Allocator;
		std::vector<Unique<D3D12CommandList>> m_CommandLists;
	};

}
