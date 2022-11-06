#pragma once

#include "Fusion/Renderer/CommandAllocator.hpp"

#include "D3D11CommandList.hpp"

namespace Fusion {

	class D3D11CommandAllocator : public CommandAllocator
	{
	public:
		D3D11CommandAllocator(D3DComPtr<ID3D11DeviceContext> InDeviceContext, const CommandAllocatorInfo& InCreateInfo);
		D3D11CommandAllocator(const CommandAllocatorInfo& InCreateInfo);
		~D3D11CommandAllocator();

		virtual CommandList* AllocateCommandList() override;
		virtual std::vector<CommandList*> AllocateCommandLists(size_t InCount) override;
		virtual CommandList* GetCommandList(size_t InIndex) const override
		{
			FUSION_CORE_VERIFY(InIndex < m_CommandLists.size());
			return m_CommandLists[InIndex].get();
		}

		virtual void Reset() override {}

	private:
		CommandList* AllocateCommandList(D3DComPtr<ID3D11DeviceContext> InDeviceContext);
		std::vector<CommandList*> AllocateCommandLists(D3DComPtr<ID3D11DeviceContext> InDeviceContext, size_t InCount);

	private:
		CommandAllocatorInfo m_CreateInfo;
		std::vector<Unique<D3D11CommandList>> m_CommandLists;
	};

}
