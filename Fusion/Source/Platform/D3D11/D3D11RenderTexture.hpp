#pragma once

#include "Fusion/Renderer/RenderTexture.hpp"

namespace Fusion {

	class D3D11RenderTexture : public RenderTexture
	{
	public:
		D3D11RenderTexture(const RenderTextureInfo& InCreateInfo);
		~D3D11RenderTexture();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Clear(const glm::vec4& InColor) override;
		virtual void Resize(uint32_t InWidth, uint32_t InHeight) override;

		virtual uint32_t GetWidth() const { return m_CreateInfo.Width; }
		virtual uint32_t GetHeight() const { return m_CreateInfo.Height; }

		virtual void* GetColorTextureID() const override { return static_cast<void*>(m_ShaderResourceView); }

	private:
		void Invalidate();

	private:
		RenderTextureInfo m_CreateInfo;

		ID3D11Texture2D* m_RenderTargetTexture = nullptr;
		ID3D11RenderTargetView* m_RenderTargetView = nullptr;
		ID3D11ShaderResourceView* m_ShaderResourceView = nullptr;

		ID3D11Texture2D* m_DepthStencilBuffer = nullptr;
		ID3D11DepthStencilView* m_DepthStencilView = nullptr;
	};

}
