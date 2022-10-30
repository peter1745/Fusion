#pragma once

#include "Fusion/Renderer/GraphicsContext.hpp"
#include "D3D11SwapChain.hpp"

#define FUSION_RELEASE_COM(InResource) if (InResource != nullptr) { InResource->Release(); InResource = nullptr; }

namespace Fusion {

	class D3D11Context : public GraphicsContext
	{
	public:
		D3D11Context();
		~D3D11Context();

		ID3D11Device* GetDevice() const { return m_Device; }
		ID3D11DeviceContext* GetDeviceContext() const { return m_DeviceContext; }

	private:
		ID3D11Device* m_Device = nullptr;
		ID3D11DeviceContext* m_DeviceContext = nullptr;
	};

}
