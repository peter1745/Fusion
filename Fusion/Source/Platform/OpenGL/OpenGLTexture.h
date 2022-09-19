#pragma once

#include "Fusion/Renderer/Texture.h"

namespace Fusion {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::filesystem::path& InFilePath);
		virtual ~OpenGLTexture2D();

		virtual void Bind(uint32_t InSlot) override;

		uint32_t GetTextureSlot() const { return m_TextureSlot; }

	private:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_Channels = 0;

		uint32_t m_TextureID = 0;
		uint32_t m_TextureSlot = 0;
	};

}
