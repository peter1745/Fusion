#pragma once

#include "Fusion/Renderer/GraphicsContext.hpp"

#include "D3D11Common.hpp"
#include "D3D11CommandAllocator.hpp"
#include "D3D11DescriptorHeap.hpp"

namespace Fusion {

	class D3D11Context : public GraphicsContext
	{
	public:
		D3D11Context();
		~D3D11Context();

		virtual Shared<CommandAllocator> GetCommandAllocator() const override { return m_Allocator; }
		virtual CommandList* GetCurrentCommandList() const override { return m_Allocator->GetCommandList(0); }
		virtual void ExecuteCommandLists(const std::vector<CommandList*>& InCommandLists) override {}

		virtual void NextFrame() override {}
		virtual void WaitForGPU() override {}

		virtual uint32_t GetCurrentFrameIndex() const override { return 0; }
		virtual uint32_t GetFramesInFlight() const override { return 1; }

		virtual Shared<DescriptorHeap> GetDescriptorHeap(EDescriptorHeapType InType) const override
		{
			FUSION_CORE_VERIFY(m_DescriptorHeaps.find(InType) != m_DescriptorHeaps.end());
			return m_DescriptorHeaps.at(InType);
		}

		auto& GetDevice() { return m_Device; }
		const auto& GetDevice() const { return m_Device; }

		auto& GetDeviceContext() { return m_DeviceContext; }
		const auto& GetDeviceContext() const { return m_DeviceContext; }

	private:
		D3DComPtr<ID3D11Device> m_Device;
		D3DComPtr<ID3D11DeviceContext> m_DeviceContext;

		std::unordered_map<EDescriptorHeapType, Shared<D3D11DescriptorHeap>> m_DescriptorHeaps;

		Shared<D3D11CommandAllocator> m_Allocator;
	};

}
