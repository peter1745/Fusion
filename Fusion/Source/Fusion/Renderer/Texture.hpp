#pragma once

#include "Fusion/Core/Core.hpp"
#include "Fusion/Memory/Shared.hpp"

#include <filesystem>

namespace Fusion {

	enum class ETextureUsage { Default, Staging };
	enum class ETextureFormat
	{
		None, RGBA8UNorm, R32UInt, R32G32UInt, D24UnormS8UInt
	};

	struct Texture2DInfo
	{
		Byte* Data = nullptr;
		uint32_t Width;
		uint32_t Height;
		uint32_t Channels;

		ETextureFormat Format = ETextureFormat::RGBA8UNorm;
		ETextureUsage Usage = ETextureUsage::Default;

		bool IsRenderTextureAttachment = false;
	};

	struct RegionCopyData
	{
		uint32_t DestinationX;
		uint32_t DestinationY;

		uint32_t SourceX;
		uint32_t SourceY;
		uint32_t SourceWidth;
		uint32_t SourceHeight;
	};

	struct TextureBuffer
	{
		Byte* Data = nullptr;
		uint32_t RowPitch = 0;

		template<typename T>
		T Read(uint32_t InX, uint32_t InY) const { return *reinterpret_cast<T*>(Data + (InX + InY * RowPitch / sizeof(T))); }
	};

	class Texture2D : public SharedObject
	{
	public:
		virtual ~Texture2D() = default;
		
		virtual void Bind(uint32_t InSlot) const = 0;

		virtual void CopyFrom(const Shared<Texture2D>& InOther) = 0;
		virtual void CopyRegion(const Shared<Texture2D>& InOther, const RegionCopyData& InRegion) = 0;

		virtual void MapRead() = 0;
		virtual TextureBuffer Read() = 0;
		virtual void Unmap() = 0;

		static Shared<Texture2D> Create(const Texture2DInfo& InCreateInfo);
	};

}
