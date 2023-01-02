#pragma once

#include "Fusion/Core/Enum.hpp"
#include "Fusion/Core/Assert.hpp"

namespace Fusion {

	enum class EShaderType
	{
		Vertex,
		Pixel
	};

	enum class EBufferUsage
	{
		Static, // Represents a buffer that *can* be updated, but isn't expected to be updated every frame
		Dynamic, // Represents a buffer that is expected to be updated every frame
		Immutable // Represents a completely immutable buffer, the GPU can only read, and the CPU can't read or write
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

	using EImageFlag = Flags<>;
	using EImageState = Flags<>;

	namespace ImageStates
	{
		static constexpr EImageState RenderTarget = (1 << 0);
		static constexpr EImageState DepthWrite = (1 << 1);
		static constexpr EImageState DepthRead = (1 << 2);
		static constexpr EImageState UnorderedAccess = (1 << 3);
		static constexpr EImageState NonPixelShaderResource = (1 << 4);
		static constexpr EImageState PixelShaderResource = (1 << 5);
		static constexpr EImageState IndirectArgument = (1 << 6);
		static constexpr EImageState CopyDst = (1 << 7);
		static constexpr EImageState CopySrc = (1 << 8);
		static constexpr EImageState ResolveDst = (1 << 9);
		static constexpr EImageState ResolveSrc = (1 << 10);
		static constexpr EImageState ShadingRateSrc = (1 << 11);
		static constexpr EImageState Undefined = (1 << 12);
	};

	namespace ImageFlags
	{
		static constexpr EImageFlag AllowRenderTarget = (1 << 0);
		static constexpr EImageFlag AllowDepthStencil = (1 << 1);
	}

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

		FUSION_CORE_VERIFY(false);
		return 0;
	}

}
