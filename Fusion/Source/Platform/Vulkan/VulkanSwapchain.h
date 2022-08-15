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

		uint32_t GetCurrentFrameIndex() const { return m_CurrentImageIndex; }
		VkExtent2D GetSwapchainExtent() const { return m_ImageExtent; }
		const std::vector<VkImageView>& GetImageViews() const { return m_ImageViews; }
		VkImage GetCurrentImage() const { return m_Images[m_CurrentImageIndex]; }
		VkImageView GetCurrentImageView() const { return m_ImageViews[m_CurrentImageIndex]; }
		VkFormat GetSwapchainFormat() const { return m_ImageFormat.format; }

		VkSemaphore GetImageAvailableSemaphore() const { return m_ImageAvailableSemaphores[m_CurrentFrame]; }
		VkSemaphore GetRenderFinishedSemaphore() const { return m_RenderFinishedSemaphores[m_CurrentFrame]; }
		VkFence GetFrameInFlightFence() const { return m_FrameInFlightFences[m_CurrentFrame]; }
		uint32_t GetMaxFramesInFlight() const { return m_MaxFramesInFlight; }
		uint32_t GetCurrentFrame() const { return m_CurrentFrame; }

		void AquireNextFrame();
		void SwapBuffers();

	private:
		VkInstance m_Instance = VK_NULL_HANDLE;
		Shared<VulkanDevice> m_Device;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkExtent2D m_ImageExtent = {};
		uint32_t m_MaxFramesInFlight = 0;
		uint32_t m_CurrentFrame = 0;

		VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
		VkSurfaceFormatKHR m_ImageFormat;
		VkPresentModeKHR m_PresentMode;
		VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;

		std::vector<VkImage> m_Images;
		std::vector<VkImageView> m_ImageViews;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_FrameInFlightFences;

		uint32_t m_CurrentImageIndex = 0;
	};

}
