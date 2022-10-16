#pragma once

#include "Fusion/Core/Core.h"

#include <initializer_list>

namespace Fusion {

	enum class ShaderDataType
	{
		Float,
		Float2,
		Float3,
		Float4,
		Mat3x3,
		Mat4x4
	};

	inline static uint32_t GetShaderDataTypeSize(ShaderDataType InType)
	{
		switch (InType)
		{
		case ShaderDataType::Float: return sizeof(float);
		case ShaderDataType::Float2: return sizeof(float) * 2;
		case ShaderDataType::Float3: return sizeof(float) * 3;
		case ShaderDataType::Float4: return sizeof(float) * 4;
		case ShaderDataType::Mat3x3: return sizeof(float) * 3 * 3;
		case ShaderDataType::Mat4x4: return sizeof(float) * 4 * 4;
		}

		FUSION_CORE_VERIFY(false, "Unknown ShaderDataType value!");
		return 0;
	}

	struct VertexBufferAttribute
	{
		const char* Name;
		ShaderDataType Type;
		uint32_t Offset;

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
			case ShaderDataType::Float: return 1;
			case ShaderDataType::Float2: return 2;
			case ShaderDataType::Float3: return 3;
			case ShaderDataType::Float4: return 4;
			}

			FUSION_CORE_VERIFY(false, "Unhandled Type!");
			return 0;
		}
	};

	class VertexBufferLayout
	{
	public:
		VertexBufferLayout() = default;
		VertexBufferLayout(const std::initializer_list<VertexBufferAttribute>& InAttributes)
			: m_Stride(0), m_Attributes(InAttributes)
		{
			for (const auto& Attribute : InAttributes)
				m_Stride += GetShaderDataTypeSize(Attribute.Type);
		}

		uint32_t GetStride() const { return m_Stride; }
		const std::vector<VertexBufferAttribute>& GetAttributes() const { return m_Attributes; }

	private:
		uint32_t m_Stride = 0;
		std::vector<VertexBufferAttribute> m_Attributes;
	};
}
