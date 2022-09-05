#pragma once

#include "Fusion/Core/Core.h"

namespace Fusion {

	class RenderCommandBuffer
	{
		typedef void(*RenderCommandFunc)(void*);
	public:
		RenderCommandBuffer();
		~RenderCommandBuffer();

		template<typename TFunc>
		void RecordCommand(TFunc&& InCommandFunc)
		{
			auto RenderCommand = [](void* InFuncPtr)
			{
				auto CommandFuncPtr = static_cast<TFunc*>(InFuncPtr);
				(*CommandFuncPtr)();
				CommandFuncPtr->~TFunc();
			};

			auto StorageBuffer = Allocate(RenderCommand, sizeof(InCommandFunc));
			new (StorageBuffer) TFunc(std::forward<TFunc>(InCommandFunc));
		}

		void Execute();

	private:
		void* Allocate(RenderCommandFunc InFunc, uint32_t InSize);

	private:
		Byte* m_CommandBuffer = nullptr;
		Byte* m_CommandBufferPtr = nullptr;

		uint32_t m_CommandCount = 0;
	};

}
