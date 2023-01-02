#pragma once

#include "VulkanCommon.hpp"
#include "VulkanDevice.hpp"
#include "Fusion/Renderer/Image.hpp"

#include <VMA/vk_mem_alloc.h>

namespace Fusion {

	class VulkanImage2D : public Image2D
	{
	public:
		VulkanImage2D(const Shared<Device>& InDevice, const Image2DInfo& InCreateInfo);

		virtual const ImageSize& GetSize() const override { return m_CreateInfo.Size; }

		virtual void Resize(const ImageSize& InSize) override
		{
			m_CreateInfo.Size = InSize;
			Invalidate();
		}

		virtual void Transition(CommandList* InCmdList, EImageState InState) override;

		virtual EImageState GetState() const override { return m_State; }

		virtual const Image2DInfo& GetInfo() const override { return m_CreateInfo; }

		void Release() override;

		VkImage GetImage() { return m_Image; }

	private:
		void Invalidate();

	private:
		Image2DInfo m_CreateInfo;

		Shared<VulkanDevice> m_Device = nullptr;

		VkImage m_Image = VK_NULL_HANDLE;
		VmaAllocation m_Allocation;
		EImageState m_State;

		friend class VulkanRenderTexture;
	};

}
