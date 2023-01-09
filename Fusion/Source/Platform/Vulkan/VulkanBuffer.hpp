#pragma once

#include "Fusion/Renderer/Buffer.hpp"

#include "VulkanCommon.hpp"
#include "VulkanContext.hpp"

namespace Fusion {

	class VulkanBuffer : public Buffer
	{
	public:
		VulkanBuffer(const BufferInfo& InInfo);
		~VulkanBuffer();

		Byte* Map() override;
		void Unmap(Byte* InPtr) override;

		void Transition(CommandList* InCmdList, EBufferState InState) override;

		void SetData(CommandList* InCmdList, const void* InData, const Shared<Buffer>& InUploadBuffer) override;

		const BufferInfo& GetInfo() const override { return m_Info; }
		uint64_t GetSize() const override { return m_Info.Size; }

		VkBuffer GetBuffer() const { return m_Buffer; }

	private:
		BufferInfo m_Info;

		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VmaAllocation m_Allocation;
	};

}
