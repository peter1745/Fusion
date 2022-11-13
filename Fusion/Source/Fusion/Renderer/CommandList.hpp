#pragma once

#include "Fusion/Memory/Shared.hpp"

#include "BufferViews.hpp"
#include "Viewport.hpp"

#include <vector>

namespace Fusion {

	// TODO(Peter): Maybe support Bundle as well?
	enum class ECommandListType { Direct, Compute };

	class Buffer;
	class DescriptorHeap;
	class Texture2D;
	class GraphicsPipeline;
	class CommandList
	{
	public:
		virtual void Reset() = 0;

		virtual void BeginRecording() = 0;
		virtual void SetViewports(const std::vector<Viewport>& InViewports) = 0;

		virtual void SetConstantBuffer(GraphicsPipeline* InPipeline, const std::string& InName, const Shared<Buffer>& InConstantBuffer) = 0;
		virtual void SetTexture(GraphicsPipeline* InPipeline, const std::string& InName, const Shared<Texture2D>& InTexture) = 0;
		virtual void SetVertexBuffer(const VertexBufferView& InBufferView) = 0;

		virtual void SetDescriptorHeaps(const std::vector<Shared<DescriptorHeap>>& InHeaps) = 0;

		virtual void DrawInstanced(uint32_t InInstanceVertexCount, uint32_t InInstanceCount, uint32_t InStartVertexLocation, uint32_t InStartInstanceLocation) = 0;
		virtual void DrawIndexed(const IndexBufferView& InBufferView) = 0;

		virtual void EndRecording() = 0;

	};

}
