#pragma once

#include "Fusion/Renderer/GraphicsContext.hpp"

#include "D3D11Device.hpp"
#include "D3D11CommandAllocator.hpp"
#include "D3D11DescriptorHeap.hpp"

namespace Fusion {

	class D3D11Context : public GraphicsContext
	{
	public:
		D3D11Context();
		~D3D11Context();

		virtual Shared<DescriptorHeap> GetDescriptorHeap(EDescriptorHeapType InType) const override
		{
			FUSION_CORE_VERIFY(m_DescriptorHeaps.find(InType) != m_DescriptorHeaps.end());
			return m_DescriptorHeaps.at(InType);
		}

		Shared<Device> GetDevice() const override { return m_Device; }

		void Release() override;

	private:
		Shared<D3D11Device> m_Device = nullptr;

		std::unordered_map<EDescriptorHeapType, Shared<D3D11DescriptorHeap>> m_DescriptorHeaps;

		Shared<D3D11CommandAllocator> m_Allocator;
	};

}
