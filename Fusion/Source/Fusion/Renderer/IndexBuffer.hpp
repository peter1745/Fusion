#pragma once

#include "Fusion/Memory/Shared.hpp"

namespace Fusion {

	class IndexBuffer : public SharedObject
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() = 0;

		virtual uint32_t GetCount() const = 0;

	public:
		static Shared<IndexBuffer> Create(uint32_t InSize, void* InData);
	};

}
