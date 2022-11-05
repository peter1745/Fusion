#pragma once

#include "Fusion/Memory/Shared.hpp"
#include "Fusion/Core/Enum.hpp"

namespace Fusion {

	enum class EBufferType
	{
		VertexBuffer, IndexBuffer, ConstantBuffer, StagingBuffer
	};

	class Buffer
	{
	public:
		virtual ~Buffer() = default;


	};

}
