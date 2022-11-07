#pragma once

#include "Fusion/Core/Core.hpp"
#include "CommonTypes.hpp"
#include "Buffer.hpp"

namespace Fusion {
	
	struct VertexBufferView
	{
		Shared<Buffer> VertexBuffer;
		uint32_t VertexStride;
	};

	struct IndexBufferView
	{
		Shared<Buffer> IndexBuffer;
		EFormat IndexFormat;
	};
	
}
