#pragma once

#include "VulkanDevice.h"
#include <GLFW/glfw3.h>

namespace Fusion {

	class VulkanSwapchain
	{
	public:
		// NOTE(Peter): Swapchain should be responsible for creating the surface
		VulkanSwapchain(VkInstance InInstance, Shared<VulkanDevice> InDevice, VkSurfaceKHR InSurface);
		~VulkanSwapchain();

		void Create();
		void InitSurface(GLFWwindow* NativeWindow);

		VkRenderPass GetRenderPass() const { return m_RenderPass; }

		VkFramebuffer GetFramebuffer(uint32_t InImageIndex) const
		{
			FUSION_CORE_VERIFY(InImageIndex < m_Framebuffers.size());
			return m_Framebuffers[InImageIndex];
		}

		uint32_t GetCurrentFrameIndex() const { return m_CurrentImageIndex; }
		VkExtent2D GetSwapchainExtent() const { return m_ImageExtent; }

		VkSemaphore GetImageAvailableSemaphore() const { return m_ImageAvailableSemaphore; }
		VkSemaphore GetRenderFinishedSemaphore() const { return m_RenderFinishedSemaphore; }
		VkFence GetFrameInFlightFence() const { return m_FrameInFlightFence; }

		void AquireNextFrame();
		void SwapBuffers();

	private:
		VkInstance m_Instance = VK_NULL_HANDLE;
		Shared<VulkanDevice> m_Device;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkExtent2D m_ImageExtent = {};
		uint32_t m_QueueIndex = UINT32_MAX;

		VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
		VkSurfaceFormatKHR m_ImageFormat;
		VkPresentModeKHR m_PresentMode;
		VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;

		VkRenderPass m_RenderPass;

		std::vector<VkImage> m_Images;
		std::vector<VkImageView> m_ImageViews;
		std::vector<VkFramebuffer> m_Framebuffers;

		VkSemaphore m_ImageAvailableSemaphore;
		VkSemaphore m_RenderFinishedSemaphore;
		VkFence m_FrameInFlightFence;

		uint32_t m_CurrentImageIndex = 0;
	};

}
