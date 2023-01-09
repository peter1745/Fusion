#pragma once

#include "Common.hpp"
#include "VulkanAllocator.hpp"

namespace Fusion {

	struct BufferInfo
	{
		EBufferUsage Usage;
		uint64_t Size;

		void* InitialData = nullptr;
	};

	class CommandBuffer;

	class Buffer : public SharedObject
	{
	public:
		Buffer(const BufferInfo& InInfo);
		~Buffer();

		Byte* Map();
		void Unmap(Byte* InPtr);

		//void Transition(CommandList* InCmdList, EBufferState InState);

		void SetData(CommandBuffer* InCmdList, const void* InData, const Shared<Buffer>& InUploadBuffer);

		const BufferInfo& GetInfo() const { return m_Info; }
		uint64_t GetSize() const { return m_Info.Size; }

		VkBuffer GetBuffer() const { return m_Buffer; }

	private:
		BufferInfo m_Info;

		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VmaAllocation m_Allocation;
	};

}
