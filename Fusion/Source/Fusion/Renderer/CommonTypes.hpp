#pragma once

#include "Fusion/Core/Enum.hpp"

namespace Fusion {

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
		RGB32Float, RGB32UInt,
		RG32Float, RG32UInt,
		R32Float, R32UInt,

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
	};

	namespace ImageFlags
	{
		static constexpr EImageFlag AllowRenderTarget = (1 << 0);
		static constexpr EImageFlag AllowDepthStencil = (1 << 1);
	}

}
