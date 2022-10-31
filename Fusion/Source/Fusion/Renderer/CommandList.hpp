#pragma once

#include "Fusion/Memory/Shared.hpp"

#include "Viewport.hpp"

#include <vector>

namespace Fusion {

	// TODO(Peter): Maybe support Bundle as well?
	enum class ECommandListType { Direct, Compute };

	class CommandList : public SharedObject
	{
	public:
		virtual void Reset() = 0;

		virtual void BeginRecording() = 0;
		virtual void SetViewports(const std::vector<Viewport>& InViewports) = 0;

		virtual void EndRecording() = 0;
	};

}
