#pragma once

#include "Fusion/Renderer/SwapChain.hpp"

#include "D3D11Common.hpp"

namespace Fusion {

	class D3D11SwapChain : public SwapChain
	{
	public:
		D3D11SwapChain(const SwapChainInfo& InCreateInfo);
		~D3D11SwapChain();

		virtual void Bind() override;
		virtual void Clear() override;
		virtual void Present() override;
		virtual void Unbind() {}

		virtual void Resize(uint32_t InWidth, uint32_t InHeight) override;

	private:
		void Invalidate();

	private:
		SwapChainInfo m_CreateInfo;

		D3DComPtr<IDXGISwapChain> m_SwapChain;
		D3DComPtr<ID3D11RenderTargetView> m_RenderTargetView;

		D3D11_VIEWPORT m_Viewport = { 0 };
	};

}
