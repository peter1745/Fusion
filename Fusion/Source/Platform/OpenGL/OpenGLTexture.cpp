#include "FusionPCH.h"
#include "OpenGLTexture.h"

#include <stb_image/stb_image.h>
#include <glad/gl.h>

namespace Fusion {

	OpenGLTexture2D::OpenGLTexture2D(const std::filesystem::path& InFilePath)
	{
		stbi_set_flip_vertically_on_load(1);

		int32_t ImageWidth, ImageHeight, ImageChannels;
		stbi_uc* ImageData = stbi_load(InFilePath.string().c_str(), &ImageWidth, &ImageHeight, &ImageChannels, 0);

		if (ImageData == nullptr)
		{
			FUSION_CORE_ERROR("Failed to load texture data from file {}", InFilePath.string());
		}
		else
		{
			m_Width = uint32_t(ImageWidth);
			m_Height = uint32_t(ImageHeight);
			m_Channels = uint32_t(ImageChannels);

			GLenum InternalFormat = 0;
			GLenum DataFormat = 0;

			if (ImageChannels == 4)
			{
				InternalFormat = GL_RGBA8;
				DataFormat = GL_RGBA;
			}
			else if (ImageChannels == 3)
			{
				InternalFormat = GL_RGB8;
				DataFormat = GL_RGB;
			}

			FUSION_CORE_VERIFY(InternalFormat & DataFormat, "Unsupported Texture Format!");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
			glTextureStorage2D(m_TextureID, 1, InternalFormat, m_Width, m_Height);

			glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, DataFormat, GL_UNSIGNED_BYTE, ImageData);

			stbi_image_free(ImageData);
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_TextureID);
	}

	void OpenGLTexture2D::Bind(uint32_t InSlot)
	{
		glBindTextureUnit(InSlot, m_TextureID);
		m_TextureSlot = InSlot;
	}

}
