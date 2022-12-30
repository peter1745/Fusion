#include "FusionPCH.hpp"
#include "D3D11Device.hpp"

namespace Fusion {

	D3D11Device::D3D11Device()
	{
		uint32_t DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef FUSION_DEBUG
		DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, DeviceFlags, nullptr, 0, D3D11_SDK_VERSION, m_Device, nullptr, m_DeviceContext);
	}

	void D3D11Device::Release()
	{
		m_DeviceContext.Release();
		m_Device.Release();
	}

}
