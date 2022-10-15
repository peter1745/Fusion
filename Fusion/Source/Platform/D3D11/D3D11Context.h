#pragma once

#include "Fusion/Renderer/GraphicsContext.h"

#define FUSION_RELEASE_COM(InResource) if (InResource != nullptr) { InResource->Release(); InResource = nullptr; }

namespace Fusion {

	class D3D11Context : public GraphicsContext
	{
	public:
		D3D11Context(const Unique<Window>& InWindow);
		~D3D11Context();

		virtual void ClearBackBuffer(const glm::vec4& InColor) override;
		virtual void Present() override;

		virtual void OnResize(uint32_t InWidth, uint32_t InHeight) override;

		ID3D11Device* GetDevice() const { return m_Device; }
		ID3D11DeviceContext* GetDeviceContext() const { return m_DeviceContext; }

	private:
		void CreateBackBufferView();

	private:
		ID3D11Device* m_Device = NULL;
		ID3D11DeviceContext* m_DeviceContext = NULL;
		IDXGISwapChain* m_DXGISwapChain = NULL;
		ID3D11RenderTargetView* m_BackBufferView = NULL;
	};

}
