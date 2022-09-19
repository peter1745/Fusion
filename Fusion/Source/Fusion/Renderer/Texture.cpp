#include "FusionPCH.h"
#include "Texture.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Fusion {

	Shared<Texture2D> Texture2D::LoadFromFile(const std::filesystem::path& InFilePath)
	{
		return Shared<OpenGLTexture2D>::Create(InFilePath);
	}

}
