#include "FusionPCH.h"
#include "OpenGLCommandBuffer.h"
#include "OpenGLVertexBuffer.h"
#include "OpenGLIndexBuffer.h"
#include "OpenGLShader.h"

#include <glad/gl.h>

namespace Fusion {

	OpenGLCommandBuffer::OpenGLCommandBuffer()
	{
		m_CommandBuffer = new Byte[10 * 1024]; // 10kb for now
		m_CommandBufferPtr = m_CommandBuffer;
	}

	OpenGLCommandBuffer::~OpenGLCommandBuffer()
	{
		m_CommandBufferPtr = nullptr;
		delete[] m_CommandBuffer;
	}

	void OpenGLCommandBuffer::Begin()
	{
		FUSION_CORE_VERIFY(!m_IsRecording);
		m_IsRecording = true;
	}

	void OpenGLCommandBuffer::CmdClear(float InRed, float InGreen, float InBlue)
	{
		RecordCommand([Red = InRed, Green = InGreen, Blue = InBlue]()
		{
			glClearColor(Red, Green, Blue, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		});
	}

	void OpenGLCommandBuffer::CmdBindVertexBuffer(const Shared<VertexBuffer>& InVertexBuffer)
	{
		FUSION_CORE_VERIFY(m_IsRecording);
		RecordCommand([GLVertexBuffer = InVertexBuffer.As<OpenGLVertexBuffer>()]()
		{
			glBindVertexArray(GLVertexBuffer->GetVertexArrayID());
			glVertexArrayVertexBuffer(GLVertexBuffer->GetVertexArrayID(), 0, GLVertexBuffer->GetBufferID(), 0, GLVertexBuffer->GetLayout().GetStride());
		});
	}

	void OpenGLCommandBuffer::CmdBindIndexBuffer(const Shared<IndexBuffer>& InIndexBuffer)
	{
		FUSION_CORE_VERIFY(m_IsRecording);
		RecordCommand([GLIndexBuffer = InIndexBuffer.As<OpenGLIndexBuffer>()]()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLIndexBuffer->GetBufferID());
		});
	}

	void OpenGLCommandBuffer::CmdBindShader(const Shared<Shader>& InShader)
	{
		FUSION_CORE_VERIFY(m_IsRecording);
		RecordCommand([GLShader = InShader.As<OpenGLShader>()]()
		{
			glUseProgram(GLShader->GetShaderID());
		});
	}

	void OpenGLCommandBuffer::CmdDrawIndexed(const Shared<IndexBuffer>& InIndexBuffer)
	{
		FUSION_CORE_VERIFY(m_IsRecording);
		RecordCommand([GLIndexBuffer = InIndexBuffer.As<OpenGLIndexBuffer>()]()
		{
			glDrawElements(GL_TRIANGLES, GLIndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
		});
	}

	void OpenGLCommandBuffer::End()
	{
		FUSION_CORE_VERIFY(m_IsRecording);
		m_IsRecording = false;
	}

	void OpenGLCommandBuffer::Execute()
	{
		Byte* Buffer = m_CommandBuffer;

		for (uint32_t i = 0; i < m_CommandCount; i++)
		{
			RenderCommandFunc CommandFunc = *(RenderCommandFunc*)Buffer;
			Buffer += sizeof(RenderCommandFunc);

			uint32_t Size = *(uint32_t*)Buffer;
			Buffer += sizeof(uint32_t);

			CommandFunc(Buffer);

			Buffer += Size;
		}

		m_CommandBufferPtr = m_CommandBuffer;
		m_CommandCount = 0;
	}

	void* OpenGLCommandBuffer::Allocate(RenderCommandFunc InFunc, uint32_t InSize)
	{
		*(RenderCommandFunc*)m_CommandBufferPtr = InFunc;
		m_CommandBufferPtr += sizeof(RenderCommandFunc);

		*(uint32_t*)m_CommandBufferPtr = InSize;
		m_CommandBufferPtr += sizeof(uint32_t);

		void* Memory = m_CommandBufferPtr;
		m_CommandBufferPtr += InSize;

		m_CommandCount++;
		return Memory;
	}

}
