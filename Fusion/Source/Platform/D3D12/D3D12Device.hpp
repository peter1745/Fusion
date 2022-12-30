#pragma once

#include "Fusion/Renderer/Device.hpp"

#include "D3D12Common.hpp"

namespace Fusion {

	class D3D12Device : public Device
	{
	public:
		D3D12Device(D3DComPtr<IDXGIFactory7> InFactory);

		auto& GetGraphicsCommandQueue() { return m_GraphicsCommandQueue; }
		const auto& GetGraphicsCommandQueue() const { return m_GraphicsCommandQueue; }

		auto& GetDevice() { return m_Device; }
		const auto& GetDevice() const { return m_Device; }

		void Wait() const override;
		void Wait(D3DComPtr<ID3D12Fence1> InFence, HANDLE InEvent, uint64_t InFenceValue);

		void Release() override;

	private:
		D3DComPtr<ID3D12Device9> m_Device;

		D3DComPtr<ID3D12CommandQueue> m_GraphicsCommandQueue;

	};

}
