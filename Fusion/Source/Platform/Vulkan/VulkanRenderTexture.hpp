#pragma once

#include "Fusion/Renderer/RenderTexture.hpp"

#include "VulkanImage.hpp"

namespace Fusion {

	class VulkanRenderTexture : public RenderTexture
	{
	public:
		VulkanRenderTexture(const RenderTextureInfo& InCreateInfo);

		void Bind(CommandList* InCommandList) override;
		void Unbind(CommandList* InCommandList) override;
		void Clear() override;

		void Resize(uint32_t InAttachmentIndex, uint32_t InFrameIndex, const ImageSize& InSize) override;

		uint32_t GetWidth() const override { return m_CreateInfo.Width; }
		uint32_t GetHeight() const override { return m_CreateInfo.Height; }

		Shared<Image2D> GetImage(uint32_t InAttachmentIndex, uint32_t InImageIndex) const override { return m_Images[InAttachmentIndex]; }
		VkImageView GetImageView(uint32_t InAttachmentIndex) const { return m_ImageViews[InAttachmentIndex]; }

		void TransitionImages(CommandList* InCommandList, EImageState InColorAttachmentState, EImageState InDepthStencilState) override;

		void Release() override;

		void* GetColorTextureID(uint32_t InColorAttachmentIdx) const override { return nullptr; }

		const RenderTextureInfo& GetInfo() const override { return m_CreateInfo; }

	private:
		void Invalidate();

	private:
		RenderTextureInfo m_CreateInfo;

		std::vector<Shared<VulkanImage2D>> m_Images;
		std::vector<VkImageView> m_ImageViews;
		std::vector<VkClearValue> m_ClearValues;

		VkFramebuffer m_FrameBuffer = VK_NULL_HANDLE;
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
	};

}
