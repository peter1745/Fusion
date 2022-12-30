#pragma once

#include "Fusion/Renderer/CommandAllocator.hpp"

#include "D3D11Device.hpp"
#include "D3D11CommandList.hpp"

namespace Fusion {

	class D3D11CommandAllocator : public CommandAllocator
	{
	public:
		D3D11CommandAllocator(const Shared<D3D11Device>& InDevice, const CommandAllocatorInfo& InCreateInfo);
		~D3D11CommandAllocator();

		virtual CommandList* AllocateCommandList() override;
		virtual std::vector<CommandList*> AllocateCommandLists(size_t InCount) override;

		virtual CommandList* GetCommandList(size_t InIndex) const override
		{
			FUSION_CORE_VERIFY(InIndex < m_CommandLists.size());
			return m_CommandLists[InIndex].get();
		}

		void DestroyCommandList(CommandList* InCommandList) override;

		virtual void Reset() override {}

		void Release() override;

	private:
		CommandAllocatorInfo m_CreateInfo;
		Shared<D3D11Device> m_Device = nullptr;
		std::vector<Unique<D3D11CommandList>> m_CommandLists;
	};

}
