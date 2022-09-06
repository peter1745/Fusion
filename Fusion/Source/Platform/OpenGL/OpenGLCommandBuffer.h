#pragma once

#include "Fusion/Renderer/CommandBuffer.h"

namespace Fusion {

	class OpenGLCommandBuffer : public CommandBuffer
	{
	public:
		OpenGLCommandBuffer();
		virtual ~OpenGLCommandBuffer();

		virtual void Begin() override;

		virtual void CmdClear(float InRed, float InGreen, float InBlue) override;
		virtual void CmdBindVertexBuffer(const Shared<VertexBuffer>& InVertexBuffer) override;
		virtual void CmdBindIndexBuffer(const Shared<IndexBuffer>& InIndexBuffer) override;
		virtual void CmdBindShader(const Shared<Shader>& InShader) override;
		virtual void CmdDrawIndexed(const Shared<IndexBuffer>& InIndexBuffer) override;

		virtual void End() override;
		virtual void Execute() override;
	private:
		typedef void(*RenderCommandFunc)(void*);

	private:
		void* Allocate(RenderCommandFunc InFunc, uint32_t InSize);

		template<typename TFunc>
		void RecordCommand(TFunc&& InCommand)
		{
			auto RenderCommand = [](void* InFuncPtr)
			{
				auto CommandFuncPtr = static_cast<TFunc*>(InFuncPtr);
				(*CommandFuncPtr)();
				CommandFuncPtr->~TFunc();
			};

			auto StorageBuffer = Allocate(RenderCommand, sizeof(InCommand));
			new(StorageBuffer) TFunc(std::forward<TFunc>(InCommand));
		}

	private:
		Byte* m_CommandBuffer = nullptr;
		Byte* m_CommandBufferPtr = nullptr;
		uint32_t m_CommandCount = 0;
		bool m_IsRecording = false;

	};

}
