#pragma once

#include "Fusion/Memory/Shared.h"

#include <vector>

namespace Fusion {

	enum class EFramebufferAttachmentFormat
	{
		None,
		RGBA8,
		Depth24Stencil8
	};

	struct FramebufferAttachment
	{
		EFramebufferAttachmentFormat Format = EFramebufferAttachmentFormat::None;
	};

	struct FramebufferSpecification
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		std::vector<FramebufferAttachment> ColorAttachments;
		FramebufferAttachment DepthAttachment;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t InNewWidth, uint32_t InNewHeight) = 0;

		virtual uint32_t GetColorAttachmentID(uint32_t InColorAttachmentIndex) const = 0;

	public:
		static Shared<Framebuffer> Create(const FramebufferSpecification& InSpecification);
	};

}
