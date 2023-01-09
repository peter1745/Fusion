#include "FusionPCH.hpp"
#include "VulkanCommandList.hpp"
#include "VulkanCommandAllocator.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanBuffer.hpp"

#include "Fusion/Renderer/CommonTypes.hpp"

namespace Fusion {

	VulkanCommandList::VulkanCommandList(VkCommandBuffer InCmdBuffer)
	    : m_CommandBuffer(InCmdBuffer)
	{
	}

	void VulkanCommandList::Reset()
	{
	}

	void VulkanCommandList::BeginRecording()
	{
		VkCommandBufferBeginInfo BeginInfo = {};
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.flags = 0;
		vkBeginCommandBuffer(m_CommandBuffer, &BeginInfo);
	}

	void VulkanCommandList::SetViewports(const std::vector<Viewport>& InViewports)
	{
		std::vector<VkViewport> Viewports(InViewports.size());
		std::vector<VkRect2D> Scissors(InViewports.size());

		for (size_t Idx = 0; Idx < InViewports.size(); Idx++)
		{
			Viewports[Idx].x = InViewports[Idx].TopLeftX;
			Viewports[Idx].y = InViewports[Idx].TopLeftY;
			Viewports[Idx].width = InViewports[Idx].Width;
			Viewports[Idx].height = InViewports[Idx].Height;
			Viewports[Idx].minDepth = InViewports[Idx].MinDepth;
			Viewports[Idx].maxDepth = InViewports[Idx].MaxDepth;

			Scissors[Idx].offset = { 0, 0 };
			Scissors[Idx].extent = { static_cast<uint32_t>(InViewports[Idx].Width), static_cast<uint32_t>(InViewports[Idx].Height) };
		}

		vkCmdSetViewport(m_CommandBuffer, 0, uint32_t(Viewports.size()), Viewports.data());
		vkCmdSetScissor(m_CommandBuffer, 0, uint32_t(Scissors.size()), Scissors.data());
	}

	void VulkanCommandList::SetPushConstants(GraphicsPipeline* InPipeline, EShaderType InShaderStage, uint32_t InSize, const void* InData)
	{
		VkPipelineLayout PipelineLayout = static_cast<VulkanGraphicsPipeline*>(InPipeline)->GetPipelineLayout();
		vkCmdPushConstants(m_CommandBuffer, PipelineLayout, ShaderTypeToVkShaderStageFlags(InShaderStage), 0, InSize, InData);
	}

	void VulkanCommandList::SetTexture(GraphicsPipeline* InPipeline, const std::string& InName, const Shared<Texture2D>& InTexture)
	{
	}

	void VulkanCommandList::SetVertexBuffer(const VertexBufferView& InBufferView)
	{
		VkBuffer Buffers[] = { InBufferView.VertexBuffer.As<VulkanBuffer>()->GetBuffer() };
		VkDeviceSize Offsets[] = { 0 };
		vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, Buffers, Offsets);
	}

	void VulkanCommandList::SetDescriptorHeaps(const std::vector<Shared<DescriptorHeap>>& InHeaps)
	{
	}

	void VulkanCommandList::DrawInstanced(uint32_t InInstanceVertexCount, uint32_t InInstanceCount, uint32_t InStartVertexLocation, uint32_t InStartInstanceLocation)
	{
	}

	void VulkanCommandList::DrawIndexed(const IndexBufferView& InBufferView)
	{
		vkCmdBindIndexBuffer(m_CommandBuffer, InBufferView.IndexBuffer.As<VulkanBuffer>()->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(m_CommandBuffer, InBufferView.IndexBuffer->GetSize() / GetFormatSize(InBufferView.IndexFormat), 1, 0, 0, 0);
	}

	void VulkanCommandList::EndRecording()
	{
		FUSION_CORE_VERIFY(vkEndCommandBuffer(m_CommandBuffer) == VK_SUCCESS);
	}

	void Fusion::VulkanCommandList::Release()
	{
		if (m_CommandBuffer == VK_NULL_HANDLE)
			return;

		vkResetCommandBuffer(m_CommandBuffer, 0);
		m_CommandBuffer = VK_NULL_HANDLE;
	}
}
