#pragma once

#include "GraphicsContext.h"
#include "VertexBufferLayout.h"
#include "CommonTypes.h"

namespace Fusion {

	struct VertexBufferInfo
	{
		uint32_t BufferSize;
		void* Data;
		EBufferUsage Usage = EBufferUsage::Static;
	};

	class VertexBuffer : public SharedObject
	{
	public:
		virtual void Resize(uint32_t InNewSize) = 0;
		virtual void SetData(void* InData, uint32_t InSize) = 0;

	public:
		static Shared<VertexBuffer> Create(const VertexBufferInfo& InCreateInfo);
	};

}
