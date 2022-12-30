#pragma once

#include "Fusion/Renderer/SwapChain.hpp"

#include "D3D11Device.hpp"

namespace Fusion {

	class D3D11SwapChain : public SwapChain
	{
	public:
		D3D11SwapChain(const Shared<GraphicsContext>& InContext, const SwapChainInfo& InCreateInfo);
		~D3D11SwapChain();

		void Bind(CommandList* InCommandList) override;
		void Clear(CommandList* InCommandList) override;
		void Present();
		void Unbind(CommandList* InCommandList) override {}

		void Resize(uint32_t InWidth, uint32_t InHeight) override;

		uint32_t GetImageCount() const override { return 1; }

		void Release() override;

	private:
		void Invalidate();

	private:
		SwapChainInfo m_CreateInfo;

		Shared<D3D11Device> m_Device = nullptr;

		D3DComPtr<IDXGISwapChain> m_SwapChain;
		D3DComPtr<ID3D11RenderTargetView> m_RenderTargetView;

		D3D11_VIEWPORT m_Viewport = { 0 };
	};

}
