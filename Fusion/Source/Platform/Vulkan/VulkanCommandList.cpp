#include "FusionPCH.hpp"
#include "VulkanCommandList.hpp"
#include "VulkanCommandAllocator.hpp"

#include "Fusion/Renderer/CommonTypes.hpp"

namespace Fusion {

	VulkanCommandList::VulkanCommandList(const Shared<VulkanCommandAllocator>& InAllocator, VkCommandBuffer InCmdBuffer)
	    : m_Allocator(InAllocator), m_CommandBuffer(InCmdBuffer)
	{
	}

	void VulkanCommandList::Reset()
	{
		vkResetCommandBuffer(m_CommandBuffer, 0);
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
		for (size_t Idx = 0; Idx < InViewports.size(); Idx++)
		{
			Viewports[Idx].x = InViewports[Idx].TopLeftX;
			Viewports[Idx].y = InViewports[Idx].TopLeftY;
			Viewports[Idx].width = InViewports[Idx].Width;
			Viewports[Idx].height = InViewports[Idx].Height;
			Viewports[Idx].minDepth = InViewports[Idx].MinDepth;
			Viewports[Idx].maxDepth = InViewports[Idx].MaxDepth;
		}

		vkCmdSetViewport(m_CommandBuffer, 0, uint32_t(Viewports.size()), Viewports.data());
	}

	void VulkanCommandList::SetConstantBuffer(GraphicsPipeline* InPipeline, const std::string& InName, const Shared<Buffer>& InConstantBuffer)
	{
	}

	void VulkanCommandList::SetTexture(GraphicsPipeline* InPipeline, const std::string& InName, const Shared<Texture2D>& InTexture)
	{
	}

	void VulkanCommandList::SetVertexBuffer(const VertexBufferView& InBufferView)
	{
	}

	void VulkanCommandList::SetDescriptorHeaps(const std::vector<Shared<DescriptorHeap>>& InHeaps)
	{
	}

	void VulkanCommandList::DrawInstanced(uint32_t InInstanceVertexCount, uint32_t InInstanceCount, uint32_t InStartVertexLocation, uint32_t InStartInstanceLocation)
	{
	}

	void VulkanCommandList::DrawIndexed(const IndexBufferView& InBufferView)
	{
		vkCmdDrawIndexed(m_CommandBuffer, InBufferView.IndexBuffer->GetSize() / GetFormatSize(InBufferView.IndexFormat), 1, 0, 0, 0);
	}

	void VulkanCommandList::EndRecording()
	{
		FUSION_CORE_VERIFY(vkEndCommandBuffer(m_CommandBuffer) == VK_SUCCESS);
	}
}
