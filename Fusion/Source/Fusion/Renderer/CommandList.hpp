#pragma once

#include "Fusion/Memory/Shared.hpp"

#include "Viewport.hpp"

#include <vector>

namespace Fusion {

	// TODO(Peter): Maybe support Bundle as well?
	enum class ECommandListType { Direct, Compute };

	class CommandList
	{
	public:
		virtual void Reset() = 0;

		virtual void BeginRecording() = 0;
		virtual void SetViewports(const std::vector<Viewport>& InViewports) = 0;

		virtual void DrawInstanced(uint32_t InInstanceVertexCount, uint32_t InInstanceCount, uint32_t InStartVertexLocation, uint32_t InStartInstanceLocation) = 0;

		virtual void EndRecording() = 0;

	};

}
