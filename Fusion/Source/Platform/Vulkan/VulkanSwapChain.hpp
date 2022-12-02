#pragma once

#include "Fusion/Renderer/SwapChain.hpp"

#include <vulkan/vulkan.h>

namespace Fusion {

	class VulkanSwapChain : public SwapChain
	{
	public:
		VulkanSwapChain(const SwapChainInfo& InCreateInfo);
		~VulkanSwapChain();

		void Bind() override;
		void Clear() override;
		void Present() override;
		void Unbind() override;

		void Resize(uint32_t InWidth, uint32_t InHeight) override;

		bool AcquireNextImage();

		uint32_t GetImageCount() const { return m_ImageCount; }
		uint32_t GetFrameCount() const { return m_FrameCount; }
		uint32_t GetFrameIndex() const { return m_CurrentFrame; }

		VkSemaphore GetImageAvailableSemaphore(uint32_t InIndex) const
		{
			FUSION_CORE_VERIFY(InIndex < m_ImageAvailableSemaphores.size());
			return m_ImageAvailableSemaphores[InIndex];
		}

		VkSemaphore GetRenderFinishedSemaphore(uint32_t InIndex) const
		{
			FUSION_CORE_VERIFY(InIndex < m_RenderFinishedSemaphores.size());
			return m_RenderFinishedSemaphores[InIndex];
		}

		VkFence GetImageFence(uint32_t InIndex) const
		{
			FUSION_CORE_VERIFY(InIndex < m_ImageFences.size());
			return m_ImageFences[InIndex];
		}

	private:
		void Create(bool InWasInvalidated);
		void InitSurface();

		void Invalidate();

	private:
		SwapChainInfo m_CreateInfo;
		VkSurfaceFormatKHR m_SurfaceFormat;
		VkPresentModeKHR m_PresentMode = VK_PRESENT_MODE_FIFO_KHR;
		VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;
		VkExtent2D m_ImageExtent;
		uint32_t m_ImageCount = 0;
		uint32_t m_CurrentImage = 0;

		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

		std::vector<VkFramebuffer> m_Framebuffers;
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;

		uint32_t m_CurrentFrame = 0;
		uint32_t m_FrameCount = 3;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_ImageFences;

		std::vector<VkImage> m_Images;
		std::vector<VkImageView> m_ImageViews;
	};

}
