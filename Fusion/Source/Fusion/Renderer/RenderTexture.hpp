#pragma once

#include "Fusion/Memory/Shared.hpp"

#include <glm/glm.hpp>

namespace Fusion {

	struct RenderTextureInfo
	{
		uint32_t Width;
		uint32_t Height;
	};

	class RenderTexture : public SharedObject
	{
	public:
		virtual ~RenderTexture() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Clear(const glm::vec4& InColor) = 0;
		virtual void Resize(uint32_t InWidth, uint32_t InHeight) = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void* GetColorTextureID() const = 0;

	public:
		static Shared<RenderTexture> Create(const RenderTextureInfo& InCreateInfo);
	};

}