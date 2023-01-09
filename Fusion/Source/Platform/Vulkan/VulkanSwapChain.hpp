#pragma once

#include "Fusion/Renderer/SwapChain.hpp"
#include "Fusion/STL/FunctionList.hpp"

#include "VulkanCommon.hpp"
#include "VulkanDevice.hpp"

namespace Fusion {

	class VulkanSwapChain : public SwapChain
	{
		using InvalidatedCallbacks = FunctionList<void(const VulkanSwapChain&)>;
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

		void Release() override;

		VkImageView GetImageView(uint32_t InImageIndex) const { return m_ImageViews[InImageIndex]; }
		VkFormat GetImageFormat() const { return m_ImageFormat; }
		VkExtent2D GetImageExtent() const { return m_ImageExtent; }

		VkImage GetImage(uint32_t InImageIndex) const { return m_Images[InImageIndex]; }

		uint32_t GetCurrentImage() const { return m_CurrentImage; }

		InvalidatedCallbacks::KeyType RegisterOnInvalidatedCallback(const InvalidatedCallbacks::FuncType& InCallback)
		{
			return m_OnInvalidatedCallbacks.AddFunction(InCallback);
		}

		void UnregisterOnInvalidatedCallback(const InvalidatedCallbacks::KeyType& InKey)
		{
			m_OnInvalidatedCallbacks.RemoveFunction(InKey);
		}

	private:
		void Create(bool InWasInvalidated);

		void Invalidate();

	private:
		SwapChainInfo m_CreateInfo;

		Shared<VulkanDevice> m_Device = nullptr;

		VkExtent2D m_ImageExtent = {};
		uint32_t m_ImageCount = 0;
		uint32_t m_CurrentImage = 0;

		VkFormat m_ImageFormat;

		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

		std::vector<VkImage> m_Images;
		std::vector<VkImageView> m_ImageViews;

		InvalidatedCallbacks m_OnInvalidatedCallbacks;
	};

}
