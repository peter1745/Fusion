#pragma once

#include "Fusion/Core/Assert.hpp"
#include "Fusion/STL/Flags.hpp"

namespace Fusion {

	enum class EShaderType
	{
		Vertex,
		Pixel
	};

	using EImageUsage = Flags<>;

	namespace ImageUsages {

		static constexpr EImageUsage Texture = (1 << 0);
		static constexpr EImageUsage Attachment = (1 << 1);
		static constexpr EImageUsage CopySource = (1 << 2);
		static constexpr EImageUsage CopyDestination = (1 << 3);

	}

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
		static constexpr EImageState PixelShaderResource = (1 << 3);
		static constexpr EImageState ColorAttachmentOptimal = (1 << 4);
		static constexpr EImageState CopySource = (1 << 5);
		static constexpr EImageState CopyDestination = (1 << 6);
		static constexpr EImageState Undefined = (1 << 7);
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

		CoreVerify(false);
		return 0;
	}

	using EBufferUsage = Flags<>;

	namespace BufferUsage
	{
		static constexpr EBufferUsage Vertex = (1 << 0);
		static constexpr EBufferUsage Index = (1 << 1);
		static constexpr EBufferUsage CopyDestination = (1 << 2);
		static constexpr EBufferUsage CopySource = (1 << 3);
	}

}
