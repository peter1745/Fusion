#pragma once

#include "Fusion/Renderer/Device.hpp"

#include "D3D11Common.hpp"

namespace Fusion {

	class D3D11Device : public Device
	{
	public:
		D3D11Device();

		void Wait() const override {}

		void Release() override;

		auto& GetDevice() { return m_Device; }
		const auto& GetDevice() const { return m_Device; }

		auto& GetDeviceContext() { return m_DeviceContext; }
		const auto& GetDeviceContext() const { return m_DeviceContext; }

	private:
		D3DComPtr<ID3D11Device> m_Device;
		D3DComPtr<ID3D11DeviceContext> m_DeviceContext;

	};

}

