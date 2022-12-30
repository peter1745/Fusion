#pragma once

#include "Fusion/Core/Window.hpp"
#include "GraphicsContext.hpp"

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

		virtual void Bind(CommandList* InCommandList) = 0;
		virtual void Clear(CommandList* InCommandList) = 0;
		virtual void Unbind(CommandList* InCommandList) = 0;

		virtual void Resize(uint32_t InWidth, uint32_t InHeight) = 0;

		virtual uint32_t GetImageCount() const = 0;

		virtual void Release() = 0;

	public:
		static Shared<SwapChain> Create(const Shared<GraphicsContext>& InContext, const SwapChainInfo& InCreateInfo);
	};

}
