#pragma once

#include "UI/ImGuiPlatformContext.hpp"

#include <Platform/Vulkan/VulkanSwapChain.hpp>

#include <vulkan/vulkan.h>

namespace FusionEditor {

	class ImGuiPlatformContextVulkan : public ImGuiPlatformContext
	{
	public:
		virtual void InitPlatform(const Fusion::Unique<Fusion::Window>& InWindow, const Fusion::Shared<Fusion::GraphicsContext>& InContext, const Fusion::Shared<Fusion::SwapChain>& InSwapChain) override;
		virtual void BeginFramePlatform(Fusion::CommandList* InCommandList) override;
		virtual void EndFramePlatform(Fusion::CommandList* InCommandList) override;
		virtual void ShutdownPlatform() override;

	private:
		void OnSwapChainInvalidated(const Fusion::VulkanSwapChain& InSwapChain);

	private:
		Fusion::Shared<Fusion::VulkanSwapChain> m_SwapChain = nullptr;
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
		std::vector<VkFramebuffer> m_FrameBuffers;
		VkDescriptorPool m_FontDescriptorPool = VK_NULL_HANDLE;
		Fusion::UUID m_SwapChainCallbackHandle = 0;
	};
}
