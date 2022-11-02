#pragma once

#include "Fusion/Memory/Shared.hpp"
#include "CommonTypes.hpp"

namespace Fusion {

	enum class EShaderBindPoint
	{
		VertexShader, PixelShader, Both
	};

	struct ConstantBufferInfo
	{
		uint32_t Size;
		EShaderBindPoint BindPoint;
		EBufferUsage Usage;
	};

	class UniformBuffer : public SharedObject
	{
	public:
		virtual ~UniformBuffer() = default;

		virtual void SetData(void* InData) = 0;

		virtual uint32_t GetSize() const = 0;
		virtual EShaderBindPoint GetBindPoint() const = 0;

	public:
		static Shared<UniformBuffer> Create(const ConstantBufferInfo& InCreateInfo);
		static Shared<UniformBuffer> Create(void* InData, uint32_t InSize, EShaderBindPoint InBindPoint, EBufferUsage InUsage = EBufferUsage::Dynamic);
	};

}
