#pragma once

#include "Fusion/Core/Core.h"
#include "Fusion/Memory/Shared.h"

#include <filesystem>

namespace Fusion {

	struct Texture2DInfo
	{
		Byte* Data = nullptr;
		uint32_t Width;
		uint32_t Height;
		uint32_t BitsPerPixel;
	};

	class Texture2D : public SharedObject
	{
	public:
		virtual ~Texture2D() = default;
		
		virtual void Bind(uint32_t InSlot) const = 0;

		static Shared<Texture2D> Create(const Texture2DInfo& InCreateInfo);
	};

}
