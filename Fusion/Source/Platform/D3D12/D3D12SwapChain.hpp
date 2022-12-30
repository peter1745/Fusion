#pragma once

#include "Fusion/Renderer/SwapChain.hpp"

#include "D3D12Device.hpp"

namespace Fusion {

	class D3D12SwapChain : public SwapChain
	{
	public:
		D3D12SwapChain(const Shared<GraphicsContext>& InContext, const SwapChainInfo& InCreateInfo);
		~D3D12SwapChain();

		void Bind(CommandList* InCommandList) override;
		void Clear(CommandList* InCommandList) override;
		void Present(D3DComPtr<ID3D12Fence1> InFence, HANDLE InEvent, uint64_t InFenceValue);
		void Unbind(CommandList* InCommandList) override;

		void Resize(uint32_t InWidth, uint32_t InHeight) override;

		uint32_t GetImageCount() const override { return m_Images.size(); }

		void Release() override;

	private:
		SwapChainInfo m_CreateInfo;

		Shared<D3D12Device> m_Device = nullptr;

		D3DComPtr<IDXGISwapChain4> m_SwapChain;
		std::vector<D3DComPtr<ID3D12Resource>> m_Images;

		D3DComPtr<ID3D12DescriptorHeap> m_RTVDescriptorHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHeapStart;
		uint32_t m_RTVHeapIncrement = 0;

		uint32_t m_CurrentImageIndex = 0;
	};

}
