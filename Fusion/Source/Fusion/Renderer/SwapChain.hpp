#pragma once

#include "Fusion/Core/Window.hpp"

namespace Fusion {

	struct SwapChainInfo
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		bool HasDepthBuffer = false;

		glm::vec4 RenderTargetClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	};

	class SwapChain : public SharedObject
	{
	public:
		virtual ~SwapChain() = default;

		virtual void Bind() = 0;
		virtual void Clear() = 0;
		virtual void Present() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t InWidth, uint32_t InHeight) = 0;

	public:
		static Shared<SwapChain> Create(const SwapChainInfo& InCreateInfo);
	};

}
