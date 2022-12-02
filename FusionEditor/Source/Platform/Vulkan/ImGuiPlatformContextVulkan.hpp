#pragma once

#include "UI/ImGuiPlatformContext.hpp"

#include <vulkan/vulkan.h>

namespace FusionEditor {

	class ImGuiPlatformContextVulkan : public ImGuiPlatformContext
	{
	public:
		virtual void InitPlatform(const Fusion::Unique<Fusion::Window>& InWindow, const Fusion::Shared<Fusion::GraphicsContext>& InContext, const Fusion::Shared<Fusion::SwapChain>& InSwapChain) override;
		virtual void BeginFramePlatform() override;
		virtual void EndFramePlatform(Fusion::CommandList* InCommandList) override;
		virtual void ShutdownPlatform() override;

	private:
		VkDescriptorPool m_FontDescriptorPool = VK_NULL_HANDLE;
	};
}
