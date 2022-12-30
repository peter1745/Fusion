#pragma once

#include "Fusion/Renderer/CommandAllocator.hpp"

#include "D3D12Device.hpp"
#include "D3D12CommandList.hpp"

namespace Fusion {

	class D3D12CommandAllocator : public CommandAllocator
	{
	public:
		D3D12CommandAllocator(const Shared<D3D12Device>& InDevice, const CommandAllocatorInfo& InCreateInfo);
		~D3D12CommandAllocator();

		CommandList* AllocateCommandList() override;
		std::vector<CommandList*> AllocateCommandLists(size_t InCount) override;
		void DestroyCommandList(CommandList* InCommandList) override;

		CommandList* GetCommandList(size_t InIndex) const override
		{
			FUSION_CORE_VERIFY(InIndex < m_CommandLists.size());
			return m_CommandLists[InIndex].get();
		}

		void Reset() override;

		void Release() override;

		auto& GetNativeAllocator() { return m_Allocator; }

	private:
		CommandAllocatorInfo m_CreateInfo;
		Shared<D3D12Device> m_Device;
		D3DComPtr<ID3D12CommandAllocator> m_Allocator;
		std::vector<Unique<D3D12CommandList>> m_CommandLists;
	};

}
