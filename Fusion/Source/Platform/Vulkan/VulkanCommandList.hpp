#pragma once

#include "Fusion/Renderer/CommandList.hpp"

#include <vulkan/vulkan.h>

namespace Fusion {

	class VulkanCommandAllocator;

	class VulkanCommandList : public CommandList
	{
	public:
		VulkanCommandList(VkCommandBuffer InCmdBuffer);
		~VulkanCommandList() override = default;

		void Reset() override;

		void BeginRecording() override;
		void SetViewports(const std::vector<Viewport>& InViewports) override;

		void SetPushConstants(GraphicsPipeline* InPipeline, EShaderType InShaderStage, uint32_t InSize, const void* InData) override;

		void SetTexture(GraphicsPipeline* InPipeline, const std::string& InName, const Shared<Texture2D>& InTexture) override;
		void SetVertexBuffer(const VertexBufferView& InBufferView) override;

		void SetDescriptorHeaps(const std::vector<Shared<DescriptorHeap>>& InHeaps) override;

		void DrawInstanced(uint32_t InInstanceVertexCount, uint32_t InInstanceCount, uint32_t InStartVertexLocation, uint32_t InStartInstanceLocation) override;
		void DrawIndexed(const IndexBufferView& InBufferView) override;

		void EndRecording() override;

		void Release() override;

		auto GetBuffer() { return m_CommandBuffer; }
		auto GetBuffer() const { return m_CommandBuffer; }

	private:
		VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
	};
}
