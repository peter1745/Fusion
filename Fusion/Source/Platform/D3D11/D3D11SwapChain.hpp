#pragma once

#include "Fusion/Renderer/SwapChain.hpp"

namespace Fusion {

	class D3D11SwapChain : public SwapChain
	{
	public:
		D3D11SwapChain(const SwapChainInfo& InCreateInfo);
		~D3D11SwapChain();

		virtual void Bind() override;
		virtual void Clear() const override;
		virtual void Present() override;
		virtual void Unbind() {}

		virtual void Resize(uint32_t InWidth, uint32_t InHeight) override;

	private:
		void Invalidate();

	private:
		SwapChainInfo m_CreateInfo;

		IDXGISwapChain* m_SwapChain = nullptr;
		ID3D11RenderTargetView* m_RenderTargetView = nullptr;

		ID3D11Texture2D* m_DepthStencilBuffer = nullptr;
		ID3D11DepthStencilView* m_DepthStencilView = nullptr;

		D3D11_VIEWPORT m_Viewport = { 0 };
	};

}
