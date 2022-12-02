#pragma once

#include "Fusion/Renderer/SwapChain.hpp"
#include "VulkanDevice.hpp"

namespace Fusion {

	class VulkanSwapChain : public SwapChain
	{
	public:
		VulkanSwapChain(const Shared<GraphicsContext>& InContext, const SwapChainInfo& InCreateInfo);
		~VulkanSwapChain();

		void Bind(CommandList* InCommandList) override;
		void Clear() override;
		void Present(VkSemaphore InFinishedSemaphore);
		void Unbind(CommandList* InCommandList) override;

		void Resize(uint32_t InWidth, uint32_t InHeight) override;

		bool AcquireNextImage(VkDevice InDevice, VkSemaphore InImageAvailableSemaphore);

		uint32_t GetMinImageCount() const { return m_SurfaceCapabilities.minImageCount; }
		uint32_t GetImageCount() const { return m_ImageCount; }

		auto GetRenderPass() { return m_RenderPass; }
		auto GetRenderPass() const { return m_RenderPass; }

		void Release() override;

	private:
		void Create(bool InWasInvalidated);
		void InitSurface();

		void Invalidate();

	private:
		SwapChainInfo m_CreateInfo;

		Shared<VulkanDevice> m_Device = nullptr;

		VkSurfaceFormatKHR m_SurfaceFormat = {};
		VkPresentModeKHR m_PresentMode = VK_PRESENT_MODE_FIFO_KHR;
		VkSurfaceCapabilitiesKHR m_SurfaceCapabilities = {};

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
