#pragma once

#include "Fusion/Memory/Shared.h"
#include "CommonTypes.h"

namespace Fusion {

	class UniformBuffer : public SharedObject
	{
	public:
		virtual ~UniformBuffer() = default;

		virtual void SetData(void* InData) = 0;

		virtual uint32_t GetSize() const = 0;

	public:
		static Shared<UniformBuffer> Create(uint32_t InSize, EBufferUsage InUsage = EBufferUsage::Dynamic);
		static Shared<UniformBuffer> Create(void* InData, uint32_t InSize, EBufferUsage InUsage = EBufferUsage::Dynamic);
	};

}
