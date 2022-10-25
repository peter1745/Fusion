#pragma once

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"

namespace Fusion {

	class CommandBuffer
	{
	public:
		virtual ~CommandBuffer() = default;

		virtual void Begin() = 0;

		virtual void CmdClear(float InRed, float InGreen, float InBlue) = 0;
		virtual void CmdBindVertexBuffer(const Shared<VertexBuffer>& InVertexBuffer) = 0;
		virtual void CmdBindIndexBuffer(const Shared<IndexBuffer>& InIndexBuffer) = 0;
		virtual void CmdBindShader(const Shared<Shader>& InShader) = 0;
		virtual void CmdDrawIndexed(const Shared<IndexBuffer>& InIndexBuffer) = 0;

		virtual void End() = 0;
		virtual void Execute() = 0;
	};

}
