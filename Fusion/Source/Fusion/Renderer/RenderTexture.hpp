#pragma once

#include "Fusion/Memory/Shared.hpp"

#include <glm/glm.hpp>

namespace Fusion {

	enum class ERenderTextureAttachmentFormat
	{
		None, RGBA8, R32UInt, R32G32UInt, Depth24
	};

	/*enum class ERenderTextureAttachmentUsage
	{
		Default
	};*/

	struct RenderTextureAttachment
	{
		ERenderTextureAttachmentFormat Format;
		//ERenderTextureAttachmentUsage Usage = ERenderTextureAttachmentUsage::Default;
		bool RequiresCPUAccess = false;
		glm::vec4 ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	};

	struct RenderTextureInfo
	{
		uint32_t Width;
		uint32_t Height;
		std::vector<RenderTextureAttachment> ColorAttachments;
		RenderTextureAttachment DepthAttachment = { ERenderTextureAttachmentFormat::Depth24 };
	};

	class RenderTexture : public SharedObject
	{
	public:
		virtual ~RenderTexture() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Clear() = 0;
		virtual void Resize(uint32_t InWidth, uint32_t InHeight) = 0;

		virtual uint64_t ReadPixel(uint32_t InAttachmentIdx, uint32_t InX, uint32_t InY) = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void* GetColorTextureID(uint32_t InColorAttachmentIdx) const = 0;

	public:
		static Shared<RenderTexture> Create(const RenderTextureInfo& InCreateInfo);
	};

}
