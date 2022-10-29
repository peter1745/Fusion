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

		virtual void Clear() override;
		virtual void Resize(uint32_t InWidth, uint32_t InHeight) override;

		virtual uint64_t ReadPixel(uint32_t InAttachmentIdx, uint32_t InX, uint32_t InY) override;

		virtual uint32_t GetWidth() const override { return m_CreateInfo.Width; }
		virtual uint32_t GetHeight() const override { return m_CreateInfo.Height; }

		virtual void* GetColorTextureID(uint32_t InColorAttachmentIdx) const override
		{
			FUSION_CORE_VERIFY(InColorAttachmentIdx < m_ColorAttachmentResourceViews.size());
			return static_cast<void*>(m_ColorAttachmentResourceViews[InColorAttachmentIdx]);
		}

	private:
		void Invalidate();

	private:
		RenderTextureInfo m_CreateInfo;
		D3D11_VIEWPORT m_Viewport;

		std::vector<ID3D11Texture2D*> m_ColorAttachmentTextures;
		std::vector<ID3D11RenderTargetView*> m_ColorAttachmentViews;
		std::vector<ID3D11ShaderResourceView*> m_ColorAttachmentResourceViews;

		std::unordered_map<size_t, ID3D11Texture2D*> m_StagingTextures;

		ID3D11Texture2D* m_DepthStencilTexture = nullptr;
		ID3D11DepthStencilView* m_DepthStencilView = nullptr;

	};

}
