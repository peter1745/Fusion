#pragma once

namespace Fusion {

	enum class EBufferUsage
	{
		Static, // Represents a buffer that *can* be updated, but isn't expected to be updated every frame
		Dynamic, // Represents a buffer that is expected to be updated every frame
		Immutable // Represents a completely immutable buffer, the GPU can only read, and the CPU can't read or write
	};

}
