#pragma once

#include "Fusion/Renderer/SwapChain.hpp"

#include "D3D12Common.hpp"

namespace Fusion {

	class D3D12SwapChain : public SwapChain
	{
	public:
		D3D12SwapChain(const SwapChainInfo& InCreateInfo);
		~D3D12SwapChain();

		virtual void Bind() override;
		virtual void Clear() const override;
		virtual void Present() override;
		virtual void Unbind() override;

		virtual void Resize(uint32_t InWidth, uint32_t InHeight) override;

	private:
		SwapChainInfo m_CreateInfo;

		D3D12ComPtr<IDXGISwapChain4> m_SwapChain;
		std::vector<D3D12ComPtr<ID3D12Resource>> m_Images;

		D3D12ComPtr<ID3D12DescriptorHeap> m_RTVDescriptorHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHeapStart;
		uint32_t m_RTVHeapIncrement = 0;

		uint32_t m_CurrentImageIndex = 0;
	};

}
