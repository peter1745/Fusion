#pragma once

#include "Fusion/Renderer/CommandAllocator.hpp"
#include "D3D12CommandList.hpp"

namespace Fusion {

	class D3D12CommandAllocator : public CommandAllocator
	{
	public:
		D3D12CommandAllocator(D3D12ComPtr<ID3D12Device9> InDevice, const CommandAllocatorInfo& InCreateInfo);
		D3D12CommandAllocator(const CommandAllocatorInfo& InCreateInfo);
		~D3D12CommandAllocator();

		virtual Shared<CommandList> AllocateCommandList() override;
		virtual std::vector<Shared<CommandList>> AllocateCommandLists(size_t InCount) override;

		virtual Shared<CommandList> GetCommandList(size_t InIndex) const override
		{
			FUSION_CORE_VERIFY(InIndex < m_CommandLists.size());
			return m_CommandLists[InIndex];
		}

		virtual void Reset() override;

		auto& GetNativeAllocator() { return m_Allocator; }

	private:
		Shared<CommandList> AllocateCommandList(D3D12ComPtr<ID3D12Device9> InDevice);
		std::vector<Shared<CommandList>> AllocateCommandLists(D3D12ComPtr<ID3D12Device9> InDevice, size_t InCount);

	private:
		CommandAllocatorInfo m_CreateInfo;
		D3D12ComPtr<ID3D12CommandAllocator> m_Allocator;
		std::vector<Shared<D3D12CommandList>> m_CommandLists;
	};

}
