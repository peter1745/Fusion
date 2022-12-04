#pragma once

#include "Fusion/Renderer/RenderTexture.hpp"

namespace Fusion {

	class VulkanRenderTexture : public RenderTexture
	{
	public:
		explicit VulkanRenderTexture(const RenderTextureInfo& InCreateInfo);

		void Bind(CommandList* InCommandList) override;
		void Unbind(CommandList* InCommandList) override;
		void Clear() override;

		void Resize(uint32_t InAttachmentIndex, uint32_t InFrameIndex, const ImageSize& InSize) override;

		uint32_t GetWidth() const override { return m_CreateInfo.Width; }
		uint32_t GetHeight() const override { return m_CreateInfo.Height; }

		Shared<Image2D> GetImage(uint32_t InAttachmentIndex, uint32_t InImageIndex) const override { return nullptr; }

		void TransitionImages(CommandList* InCommandList, EImageState InColorAttachmentState, EImageState InDepthStencilState) override;

		void* GetColorTextureID(uint32_t InColorAttachmentIdx) const override { return nullptr; }

		const RenderTextureInfo& GetInfo() const override { return m_CreateInfo; }

	private:
		RenderTextureInfo m_CreateInfo;
	};

}
