#pragma once

namespace Fusion {

	enum class EBufferUsage
	{
		Static, // Represents a buffer that *can* be updated, but isn't expected to be updated every frame
		Dynamic, // Represents a buffer that is expected to be updated every frame
		Immutable // Represents a completely immutable buffer, the GPU can only read, and the CPU can't read or write
	};

	enum class EGraphicsFormat
	{
		Unknown,
		RGBA32Float, RGBA32UInt,
		RGB32Float, RGB32UInt,
		RG32Float, RG32UInt,
		R32Float, R32UInt,

		RGBA8Unorm, RGBA8UInt,

		D24UnormS8UInt
	};

	static constexpr bool IsDepthFormat(EGraphicsFormat InFormat)
	{
		return InFormat == EGraphicsFormat::D24UnormS8UInt;
	}

}
