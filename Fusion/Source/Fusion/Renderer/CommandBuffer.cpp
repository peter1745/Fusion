#include "FusionPCH.hpp"
#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "GraphicsPipeline.hpp"
#include "Buffer.hpp"

#include "Fusion/Renderer/CommonTypes.hpp"

namespace Fusion {

	CommandBuffer::CommandBuffer(VkCommandBuffer InCmdBuffer)
	    : m_CommandBuffer(InCmdBuffer)
	{
	}

	void CommandBuffer::Reset()
	{
	}

	void CommandBuffer::BeginRecording()
	{
		VkCommandBufferBeginInfo BeginInfo = {};
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.flags = 0;
		vkBeginCommandBuffer(m_CommandBuffer, &BeginInfo);
	}

	void CommandBuffer::SetViewports(const std::vector<Viewport>& InViewports)
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

	void CommandBuffer::SetPushConstants(GraphicsPipeline* InPipeline, EShaderType InShaderStage, uint32_t InSize, const void* InData)
	{
		VkPipelineLayout PipelineLayout = static_cast<GraphicsPipeline*>(InPipeline)->GetPipelineLayout();
		vkCmdPushConstants(m_CommandBuffer, PipelineLayout, ShaderTypeToVkShaderStageFlags(InShaderStage), 0, InSize, InData);
	}

	/*void CommandBuffer::SetTexture(GraphicsPipeline* InPipeline, const std::string& InName, const Shared<Texture2D>& InTexture)
	{
	}*/

	void CommandBuffer::SetVertexBuffer(const VertexBufferView& InBufferView)
	{
		VkBuffer Buffers[] = { InBufferView.VertexBuffer.As<Buffer>()->GetBuffer() };
		VkDeviceSize Offsets[] = { 0 };
		vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, Buffers, Offsets);
	}

	void CommandBuffer::DrawInstanced(uint32_t InInstanceVertexCount, uint32_t InInstanceCount, uint32_t InStartVertexLocation, uint32_t InStartInstanceLocation)
	{
	}

	void CommandBuffer::DrawIndexed(const IndexBufferView& InBufferView)
	{
		vkCmdBindIndexBuffer(m_CommandBuffer, InBufferView.IndexBuffer.As<Buffer>()->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(m_CommandBuffer, InBufferView.IndexBuffer->GetSize() / GetFormatSize(InBufferView.IndexFormat), 1, 0, 0, 0);
	}

	void CommandBuffer::EndRecording()
	{
		FUSION_CORE_VERIFY(vkEndCommandBuffer(m_CommandBuffer) == VK_SUCCESS);
	}

	void CommandBuffer::Release()
	{
		if (m_CommandBuffer == VK_NULL_HANDLE)
			return;

		vkResetCommandBuffer(m_CommandBuffer, 0);
		m_CommandBuffer = VK_NULL_HANDLE;
	}
}
