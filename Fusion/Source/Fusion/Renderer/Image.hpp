#pragma once

#include "CommonTypes.hpp"
#include "CommandList.hpp"

#include "Fusion/Memory/Shared.hpp"

#include <glm/glm.hpp>

namespace Fusion {

	enum class EImageUsage { Texture, Attachment };

	struct ImageSize { uint32_t Width; uint32_t Height; };

	struct Image2DInfo
	{
		ImageSize Size;

		EImageUsage Usage;
		EFormat Format;
		EImageFlag Flags;
		EImageState InitialState;

		// Only used for render targets and depth stencils
		glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		uint32_t Channels = 0;
		Byte* InitialData = nullptr;
	};

	class Image2D : public SharedObject
	{
	public:
		virtual ~Image2D() = default;

		virtual const ImageSize& GetSize() const = 0;

		virtual void Resize(const ImageSize& InSize) = 0;

		// NOTE(Peter): For RenderTextures we will most likely transition the current images for ALL attachments at once
		//				in one batch. Most of the time it will result in a slight optimization
		virtual void Transition(CommandList* InCmdList, EImageState InState) = 0;

		virtual EImageState GetState() const = 0;

		virtual const Image2DInfo& GetInfo() const = 0;

		virtual void Release() = 0;

	public:
		static Shared<Image2D> Create(const Image2DInfo& InCreateInfo);
	};

}
