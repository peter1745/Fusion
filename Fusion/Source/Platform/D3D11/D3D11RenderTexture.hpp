#pragma once

#include "Fusion/Renderer/RenderTexture.hpp"

#include "D3D11Common.hpp"
#include "D3D11Image.hpp"

namespace Fusion {

	class D3D11RenderTexture : public RenderTexture
	{
	public:
		D3D11RenderTexture(const RenderTextureInfo& InCreateInfo);
		~D3D11RenderTexture();

		virtual void Bind(CommandList* InCommandList) override;
		virtual void Unbind(CommandList* InCommandList) override;

		virtual void Clear() override;
	
		virtual void Resize(uint32_t InAttachmentIndex, uint32_t InFrameIndex, const ImageSize& InSize) override;

		virtual uint32_t GetWidth() const override { return m_CreateInfo.Width; }
		virtual uint32_t GetHeight() const override { return m_CreateInfo.Height; }

		virtual Shared<Image2D> GetImage(uint32_t InAttachmentIndex, uint32_t InImageIndex) const override;

		virtual void TransitionImages(CommandList* InCommandList, EImageState InColorAttachmentState, EImageState InDepthStencilState) override {}

		virtual void* GetColorTextureID(uint32_t InColorAttachmentIdx) const override
		{
			FUSION_CORE_VERIFY(InColorAttachmentIdx < m_ColorAttachmentResourceViews.size());
			return static_cast<void*>(m_ColorAttachmentResourceViews[InColorAttachmentIdx]);
		}

		virtual const RenderTextureInfo& GetInfo() const override { return m_CreateInfo; }

	private:
		void Invalidate();

	private:
		RenderTextureInfo m_CreateInfo;
		std::vector<Shared<D3D11Image2D>> m_AttachmentImages;
		std::vector<D3DComPtr<ID3D11RenderTargetView>> m_AttachmentRenderTargetViews;
		Shared<D3D11Image2D> m_DepthStencilImage;
		D3DComPtr<ID3D11DepthStencilView> m_DepthStencilView;
		bool m_HasDepthStencilAttachment = false;

		std::vector<ID3D11ShaderResourceView*> m_ColorAttachmentResourceViews;

	};

}
