#pragma once

#include "Image.hpp"

#include <glm/glm.hpp>

#include <vector>

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
		virtual ~RenderTexture() = default;

		virtual void Bind(CommandList* InCommandList) = 0;
		virtual void Unbind(CommandList* InCommandList) = 0;
		virtual void Clear() = 0;

		virtual void Resize(uint32_t InAttachmentIndex, uint32_t InFrameIndex, const ImageSize& InSize) = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual Shared<Image2D> GetImage(uint32_t InAttachmentIndex, uint32_t InImageIndex) const = 0;

		virtual void TransitionImages(CommandList* InCommandList, EImageState InColorAttachmentState, EImageState InDepthStencilState) = 0;

		virtual void Release() = 0;

		virtual void* GetColorTextureID(uint32_t InColorAttachmentIdx) const = 0;

		virtual const RenderTextureInfo& GetInfo() const = 0;

	public:
		static Shared<RenderTexture> Create(const RenderTextureInfo& InCreateInfo);
	};

}
