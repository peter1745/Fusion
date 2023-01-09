#pragma once

#include "UI/ImGuiPlatformContext.hpp"

#include <Fusion/Renderer/SwapChain.hpp>

#include <vulkan/vulkan.h>

namespace FusionEditor {

	class ImGuiPlatformContextVulkan : public ImGuiPlatformContext
	{
	public:
		virtual void InitPlatform(const Fusion::Unique<Fusion::Window>& InWindow, const Fusion::Shared<Fusion::GraphicsContext>& InContext, const Fusion::Shared<Fusion::SwapChain>& InSwapChain) override;
		virtual void BeginFramePlatform(Fusion::CommandBuffer* InCommandList) override;
		virtual void EndFramePlatform(Fusion::CommandBuffer* InCommandList) override;
		virtual void ShutdownPlatform() override;

	private:
		void OnSwapChainInvalidated(const Fusion::SwapChain& InSwapChain);

	private:
		Fusion::Shared<Fusion::SwapChain> m_SwapChain = nullptr;
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
		std::vector<VkFramebuffer> m_FrameBuffers;
		VkDescriptorPool m_FontDescriptorPool = VK_NULL_HANDLE;
		Fusion::UUID m_SwapChainCallbackHandle = 0;
	};
}
