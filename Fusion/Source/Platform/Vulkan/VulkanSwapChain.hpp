#pragma once

#include "Fusion/Renderer/SwapChain.hpp"
#include "VulkanDevice.hpp"

namespace Fusion {

	class VulkanSwapChain : public SwapChain
	{
	public:
		VulkanSwapChain(const Shared<GraphicsContext>& InContext, const SwapChainInfo& InCreateInfo);
		~VulkanSwapChain() override = default;

		void Bind(CommandList* InCommandList) override;
		void Clear(CommandList* InCommandList) override;
		void Present(VkSemaphore InFinishedSemaphore);
		void Unbind(CommandList* InCommandList) override;

		void Resize(uint32_t InWidth, uint32_t InHeight) override;

		bool AcquireNextImage(VkDevice InDevice, VkSemaphore InImageAvailableSemaphore);

		uint32_t GetImageCount() const override { return m_ImageCount; }

		auto GetRenderPass() { return m_RenderPass; }
		auto GetRenderPass() const { return m_RenderPass; }

		void Release() override;

	private:
		void Create(bool InWasInvalidated);

		void Invalidate();

	private:
		SwapChainInfo m_CreateInfo;

		Shared<VulkanDevice> m_Device = nullptr;

		VkExtent2D m_ImageExtent = {};
		uint32_t m_ImageCount = 0;
		uint32_t m_CurrentImage = 0;

		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

		std::vector<VkFramebuffer> m_FrameBuffers;

		VkRenderPass m_RenderPass = VK_NULL_HANDLE;

		std::vector<VkImage> m_Images;
		std::vector<VkImageView> m_ImageViews;
	};

}
