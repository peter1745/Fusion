#include "FusionPCH.h"
#include "OpenGLFramebuffer.h"

#include <glad/gl.h>

namespace Fusion {

	static GLenum AttachmentFormatToGLFormat(EFramebufferAttachmentFormat InFormat)
	{
		switch (InFormat)
		{
		case EFramebufferAttachmentFormat::None: return GL_NONE;
		case EFramebufferAttachmentFormat::RGBA8: return GL_RGBA8;
		case EFramebufferAttachmentFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
		}
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& InSpecification)
		: m_Specification(InSpecification)
	{
		Recreate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_FramebufferID);
		glDeleteTextures(m_ColorAttchmentIDs.size(), m_ColorAttchmentIDs.data());
		glDeleteTextures(1, &m_DepthAttachmentID);
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Recreate()
	{
		if (m_FramebufferID)
		{
			glDeleteFramebuffers(1, &m_FramebufferID);
			glDeleteTextures(m_ColorAttchmentIDs.size(), m_ColorAttchmentIDs.data());
			glDeleteTextures(1, &m_DepthAttachmentID);

			m_ColorAttchmentIDs.clear();
			m_DepthAttachmentID = 0;
		}

		glCreateFramebuffers(1, &m_FramebufferID);

		m_ColorAttchmentIDs.resize(m_Specification.ColorAttachments.size());
		for (uint32_t i = 0; i < m_Specification.ColorAttachments.size(); i++)
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttchmentIDs[i]);
			glTextureParameteri(m_ColorAttchmentIDs[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_ColorAttchmentIDs[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureStorage2D(m_ColorAttchmentIDs[i], 1, AttachmentFormatToGLFormat(m_Specification.ColorAttachments[i].Format), m_Specification.Width, m_Specification.Height);
			glNamedFramebufferTexture(m_FramebufferID, GL_COLOR_ATTACHMENT0 + i, m_ColorAttchmentIDs[i], 0);
			glNamedFramebufferDrawBuffer(m_FramebufferID, GL_COLOR_ATTACHMENT0 + i);
		}

		if (m_Specification.DepthAttachment.Format != EFramebufferAttachmentFormat::None)
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachmentID);
			glTextureParameteri(m_DepthAttachmentID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_DepthAttachmentID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureStorage2D(m_DepthAttachmentID, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
			glNamedFramebufferTexture(m_FramebufferID, GL_DEPTH_STENCIL_ATTACHMENT, m_DepthAttachmentID, 0);
		}

		FUSION_CORE_VERIFY(glCheckNamedFramebufferStatus(m_FramebufferID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}

}