#include "FusionPCH.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLCommandBuffer.h"

namespace Fusion {

	struct RendererData
	{
		Shared<CommandBuffer> ActiveCommandBuffer;
	};

	static RendererData* s_Data = nullptr;
	void Renderer::Initialize()
	{
		s_Data = new RendererData();
		s_Data->ActiveCommandBuffer = Shared<OpenGLCommandBuffer>::Create();
	}

	void Renderer::Shutdown()
	{
		delete s_Data;
	}

	void Renderer::Begin()
	{
		s_Data->ActiveCommandBuffer->Begin();
		s_Data->ActiveCommandBuffer->CmdClear(0.1f, 0.1f, 0.1f);
	}

	void Renderer::DrawIndexed(const Shared<VertexBuffer>& InVertexBuffer, const Shared<IndexBuffer>& InIndexBuffer, const Shared<Shader>& InShader)
	{
		s_Data->ActiveCommandBuffer->CmdBindVertexBuffer(InVertexBuffer);
		s_Data->ActiveCommandBuffer->CmdBindIndexBuffer(InIndexBuffer);
		s_Data->ActiveCommandBuffer->CmdBindShader(InShader);
		s_Data->ActiveCommandBuffer->CmdDrawIndexed(InIndexBuffer);
	}

	void Renderer::Clear()
	{
		s_Data->ActiveCommandBuffer->CmdClear(0.1f, 0.1f, 0.1f);
	}

	void Renderer::End()
	{
		s_Data->ActiveCommandBuffer->End();
		s_Data->ActiveCommandBuffer->Execute();
	}

	Shared<CommandBuffer> Renderer::GetActiveCommandBuffer() { return s_Data->ActiveCommandBuffer.As<CommandBuffer>(); }

}
