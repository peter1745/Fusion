#include "FusionPCH.h"
#include "Shader.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Fusion {

	Shared<Shader> Shader::Create(const ShaderSpecification& InSpecification)
	{
		return Shared<OpenGLShader>::Create(InSpecification);
	}

}
