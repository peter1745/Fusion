#pragma once

#include "Fusion/Renderer/Framebuffer.h"

#include <unordered_map>

namespace Fusion {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& InSpecification);
		virtual ~OpenGLFramebuffer();

		virtual void Bind() override;
		virtual void Unbind() override;
		virtual uint32_t GetColorAttachmentID(uint32_t InColorAttachmentIndex) const
		{
			FUSION_CORE_VERIFY(InColorAttachmentIndex < m_ColorAttchmentIDs.size());
			return m_ColorAttchmentIDs[InColorAttachmentIndex];
		}

		void Recreate();

	private:
		FramebufferSpecification m_Specification;

		uint32_t m_FramebufferID = 0;
		std::vector<uint32_t> m_ColorAttchmentIDs;
		uint32_t m_DepthAttachmentID = 0;
	};

}
