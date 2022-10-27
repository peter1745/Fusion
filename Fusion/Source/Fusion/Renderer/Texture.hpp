#pragma once

#include "Fusion/Core/Core.hpp"
#include "Fusion/Memory/Shared.hpp"

#include <filesystem>

namespace Fusion {

	struct Texture2DInfo
	{
		Byte* Data = nullptr;
		uint32_t Width;
		uint32_t Height;
		uint32_t Channels;
	};

	class Texture2D : public SharedObject
	{
	public:
		virtual ~Texture2D() = default;
		
		virtual void Bind(uint32_t InSlot) const = 0;

		static Shared<Texture2D> Create(const Texture2DInfo& InCreateInfo);
	};

}