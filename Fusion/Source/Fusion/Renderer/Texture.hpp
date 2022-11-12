#pragma once

#include "Fusion/Core/Core.hpp"
#include "Fusion/Memory/Shared.hpp"
#include "CommonTypes.hpp"
#include "Image.hpp"

#include <filesystem>

namespace Fusion {

	struct Texture2DInfo
	{
		Byte* Data = nullptr;
		uint32_t Width;
		uint32_t Height;
		uint32_t Channels;
		EFormat Format;
	};

	class Texture2D : public SharedObject
	{
	public:
		virtual ~Texture2D() = default;
		
		virtual void Bind(uint32_t InSlot) = 0;

		virtual Shared<Image2D> GetImage() const = 0;

		virtual const Texture2DInfo& GetInfo() const = 0;

	public:
		static Shared<Texture2D> Create(const Texture2DInfo& InCreateInfo);
	};

}
