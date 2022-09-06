#pragma once

#include "VertexBufferLayout.h"

namespace Fusion {

	class VertexBuffer
	{
	public:
		virtual const VertexBufferLayout& GetLayout() const = 0;

	public:
		static Shared<VertexBuffer> Create(uint32_t InSize, void* InData, const VertexBufferLayout& InLayout);
	};

}
