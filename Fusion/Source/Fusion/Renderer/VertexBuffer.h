#pragma once

#include "GraphicsContext.h"
#include "VertexBufferLayout.h"

namespace Fusion {

	enum class EBufferUsage
	{
		Static, // Represents a buffer that *can* be updated, but isn't expected to be updated every frame
		Dynamic, // Represents a buffer that is expected to be updated every frame
		Immutable // Represents a completely immutable buffer, the GPU can only read, and the CPU can't read or write
	};

	struct VertexBufferInfo
	{
		uint32_t BufferSize;
		void* Data;
		VertexBufferLayout Layout;
		EBufferUsage Usage = EBufferUsage::Static;
	};

	class VertexBuffer : public SharedObject
	{
	public:
		virtual void Resize(uint32_t InNewSize) = 0;
		virtual void SetData(void* InData, uint32_t InSize) = 0;

		virtual const VertexBufferLayout& GetLayout() const = 0;

	public:
		static Shared<VertexBuffer> Create(const VertexBufferInfo& InCreateInfo);
	};

}
