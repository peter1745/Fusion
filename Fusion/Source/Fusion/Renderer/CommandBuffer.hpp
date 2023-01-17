#pragma once

#include "Common.hpp"
#include "Viewport.hpp"
#include "BufferViews.hpp"

namespace Fusion {

	class CommandPool;
	class GraphicsPipeline;
	class Texture2D;

	class CommandBuffer
	{
	public:
		CommandBuffer(VkCommandBuffer InCmdBuffer);
		~CommandBuffer() = default;

		void Reset();

		void BeginRecording();
		void SetViewports(const std::vector<Viewport>& InViewports);

		void SetPushConstants(GraphicsPipeline* InPipeline, EShaderStage InShaderStage, uint32_t InSize, const void* InData);
		void SetUniformBuffer(GraphicsPipeline* InPipeline, const std::string& InName, const Shared<Buffer>& InBuffer, uint32_t InFrameIndex);
		void SetTexture(GraphicsPipeline* InPipeline, const std::string& InName, const Shared<Texture2D>& InTexture, uint32_t InFrameIndex);
		void SetVertexBuffer(const VertexBufferView& InBufferView);

		void DrawInstanced(uint32_t InInstanceVertexCount, uint32_t InInstanceCount, uint32_t InStartVertexLocation, uint32_t InStartInstanceLocation);
		void DrawIndexed(const IndexBufferView& InBufferView);

		void EndRecording();

		void Release();

		auto GetBuffer() { return m_CommandBuffer; }
		auto GetBuffer() const { return m_CommandBuffer; }

	private:
		VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
	};
}
