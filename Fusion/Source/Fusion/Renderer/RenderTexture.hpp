#pragma once

#include "Image.hpp"

namespace Fusion {

	struct RenderTextureAttachment
	{
		EFormat Format;
		EImageFlag Flags;
		EImageState InitialState;
		glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	};

	struct RenderTextureInfo
	{
		uint32_t Width;
		uint32_t Height;
		// TODO(Peter): std::array<RenderTextureAttachment, 8>? Do we ever need more than 8 color attachments per render texture???
		std::vector<RenderTextureAttachment> ColorAttachments;
		RenderTextureAttachment DepthAttachment = { EFormat::Unknown };
	};

	class RenderTexture : public SharedObject
	{
	public:
		RenderTexture(const RenderTextureInfo& InCreateInfo);

		void Bind(CommandBuffer* InCommandList);
		void Unbind(CommandBuffer* InCommandList);
		void Clear();

		void Resize(uint32_t InAttachmentIndex, const ImageSize& InSize);

		uint32_t GetWidth() const { return m_CreateInfo.Width; }
		uint32_t GetHeight() const { return m_CreateInfo.Height; }

		Shared<Image2D> GetImage(uint32_t InAttachmentIndex) const { return m_Images[InAttachmentIndex]; }
		VkImageView GetImageView(uint32_t InAttachmentIndex) const { return m_ImageViews[InAttachmentIndex]; }

		void TransitionImages(CommandBuffer* InCommandList, EImageState InColorAttachmentState, EImageState InDepthStencilState);

		void Release();

		void* GetColorTextureID(uint32_t InColorAttachmentIdx) const { return nullptr; }

		const RenderTextureInfo& GetInfo() const { return m_CreateInfo; }

	private:
		void Invalidate();

	private:
		RenderTextureInfo m_CreateInfo;

		std::vector<Shared<Image2D>> m_Images;
		std::vector<VkImageView> m_ImageViews;
		std::vector<VkClearValue> m_ClearValues;
	};

}
