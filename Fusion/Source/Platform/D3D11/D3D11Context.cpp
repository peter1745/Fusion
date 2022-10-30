#include "FusionPCH.hpp"
#include "D3D11Context.hpp"

#include "Fusion/Core/Application.hpp"

namespace Fusion {

	D3D11Context::D3D11Context()
	{
		s_CurrentContext = this;

		uint32_t DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef FUSION_DEBUG
		DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, DeviceFlags, nullptr, 0, D3D11_SDK_VERSION, &m_Device, nullptr, &m_DeviceContext);
	}

	D3D11Context::~D3D11Context()
	{
		FUSION_RELEASE_COM(m_Device);
		FUSION_RELEASE_COM(m_DeviceContext);
	}

}
