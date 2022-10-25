#pragma once

#include "GraphicsContext.hpp"
#include "VertexBufferLayout.hpp"
#include "CommonTypes.hpp"

namespace Fusion {

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
		virtual void Bind() const = 0;
		virtual void Resize(uint32_t InNewSize) = 0;
		virtual void SetData(void* InData, uint32_t InSize) = 0;

	public:
		static Shared<VertexBuffer> Create(const VertexBufferInfo& InCreateInfo);
	};

}
