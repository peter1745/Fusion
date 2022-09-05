#include "FusionPCH.h"
#include "OpenGLVertexBuffer.h"

#include <glad/gl.h>

namespace Fusion {

	static GLenum ShaderDataTypeToGLType(ShaderDataType InType)
	{
		switch (InType)
		{
		case Fusion::ShaderDataType::Float:
		case Fusion::ShaderDataType::Float2:
		case Fusion::ShaderDataType::Float3:
		case Fusion::ShaderDataType::Float4:
		case Fusion::ShaderDataType::Mat3x3:
		case Fusion::ShaderDataType::Mat4x4:
			return GL_FLOAT;
		}
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t InSize, void* InData, const VertexBufferLayout& InLayout)
		: m_Layout(InLayout)
	{
		glCreateVertexArrays(1, &m_VertexArrayID);
		glCreateBuffers(1, &m_BufferID);
		glNamedBufferData(m_BufferID, InSize, InData, GL_STATIC_DRAW);

		const auto& Attributes = InLayout.GetAttributes();

		uint32_t BufferedOffset = 0;
		for (uint32_t i = 0; i < Attributes.size(); i++)
		{
			BufferedOffset += Attributes[i].Offset;

			glEnableVertexArrayAttrib(m_VertexArrayID, i);
			glVertexArrayAttribFormat(m_VertexArrayID, i, Attributes[i].GetComponentCount(), ShaderDataTypeToGLType(Attributes[i].Type), GL_FALSE, Attributes[i].Offset);
			glVertexArrayAttribBinding(m_VertexArrayID, i, 0);
		}
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
	}

	void OpenGLVertexBuffer::Bind()
	{
		glBindVertexArray(m_VertexArrayID);
		glVertexArrayVertexBuffer(m_VertexArrayID, 0, m_BufferID, 0, m_Layout.GetStride());
	}

	void OpenGLVertexBuffer::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

}
