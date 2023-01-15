#pragma once

#include "Fusion/Core/Core.hpp"
#include "Fusion/Core/Assert.hpp"

#include <FTL/EnumBitmask.hpp>

namespace Fusion {

	enum class EShaderType
	{
		Vertex,
		Pixel
	};

	enum class EImageUsage
	{
		Texture = Shift1(0),
		Attachment = Shift1(1),
		CopySource = Shift1(2),
		CopyDestination = Shift1(3),
		ScopedBitmaskMark
	};

	enum class EFormat
	{
		Unknown,
		RGBA32Float, RGBA32UInt,
		RGB32Float,  RGB32UInt,
		RG32Float,   RG32UInt,
		R32Float,    R32UInt,

		RGBA32SInt,
		RGB32SInt,
		RG32SInt,
		R32SInt,

		RGBA8Unorm, RGBA8UInt,

		D24UnormS8UInt
	};

	static constexpr bool IsDepthFormat(EFormat InFormat)
	{
		return InFormat == EFormat::D24UnormS8UInt;
	}

	enum class EImageFlag
	{
		AllowRenderTarget = Shift1(0),
		AllowDepthStencil = Shift1(1),
		ScopedBitmaskMark
	};

	enum class EImageState
	{
		RenderTarget = Shift1(0),
		DepthWrite = Shift1(1),
		DepthRead = Shift1(2),
		PixelShaderResource = Shift1(3),
		ColorAttachmentOptimal = Shift1(4),
		CopySource = Shift1(5),
		CopyDestination = Shift1(6),
		Undefined = Shift1(7),
		ScopedBitmaskMark
	};

	enum class EShaderVisibility
	{
		All,
		Vertex,
		Pixel
	};

	enum class EFilterMode
	{
		Nearest, Linear
	};

	enum class EImageAddressMode { Wrap, Mirror, Clamp, Border, MirrorOnce };

	static constexpr size_t GetFormatSize(EFormat InFormat)
	{
		switch (InFormat)
		{
		case EFormat::Unknown: return 0;
		case EFormat::RGBA32Float: return 4 * sizeof(float);
		case EFormat::RGBA32UInt: return 4 * sizeof(uint32_t);
		case EFormat::RGB32Float: return 3 * sizeof(float);
		case EFormat::RGB32UInt: return 3 * sizeof(uint32_t);
		case EFormat::RG32Float: return 2 * sizeof(float);
		case EFormat::RG32UInt: return 2 * sizeof(uint32_t);
		case EFormat::R32Float: return 1 * sizeof(float);
		case EFormat::R32UInt: return 1 * sizeof(uint32_t);
		case EFormat::RGBA8Unorm:
		case EFormat::RGBA8UInt:
		case EFormat::D24UnormS8UInt:
			return 4 * sizeof(uint8_t);
		case EFormat::RGBA32SInt: return 4 * sizeof(int32_t);
		case EFormat::RGB32SInt: return 3 * sizeof(int32_t);
		case EFormat::RG32SInt: return 2 * sizeof(int32_t);
		case EFormat::R32SInt: return 1 * sizeof(int32_t);
		}

		CoreVerify(false);
		return 0;
	}

	enum class EBufferUsage
	{
		Vertex = Shift1(0),
		Index = Shift1(1),
		CopyDestination = Shift1(2),
		CopySource = Shift1(3),
		ScopedBitmaskMark
	};

}
