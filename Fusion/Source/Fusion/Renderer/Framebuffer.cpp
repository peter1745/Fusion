#include "FusionPCH.h"
#include "Framebuffer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Fusion {

	Shared<Framebuffer> Framebuffer::Create(const FramebufferSpecification& InSpecification)
	{
		return Shared<OpenGLFramebuffer>::Create(InSpecification);
	}

}
