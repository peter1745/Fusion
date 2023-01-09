#pragma once

#include "Common.hpp"
#include "Device.hpp"
#include "CommandBuffer.hpp"

#include <VMA/vk_mem_alloc.h>

namespace Fusion {

	struct ImageSize
	{
		uint32_t Width;
		uint32_t Height;
	};

	struct Image2DInfo
	{
		ImageSize Size;

		EImageUsage Usage;
		EFormat Format;
		EImageFlag Flags;
		EImageState InitialState;

		// Only used for render targets and depth stencils
		glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		uint32_t Channels = 0;
		Byte* InitialData = nullptr;
	};

	class Image2D : public SharedObject
	{
	public:
		Image2D(const Image2DInfo& InCreateInfo);

		virtual const ImageSize& GetSize() const { return m_CreateInfo.Size; }

		virtual void Resize(const ImageSize& InSize)
		{
			m_CreateInfo.Size = InSize;
			Invalidate();
		}

		virtual void Transition(CommandBuffer* InCmdList, EImageState InState);

		virtual EImageState GetState() const { return m_State; }

		virtual const Image2DInfo& GetInfo() const { return m_CreateInfo; }

		void Release();

		VkImage GetImage() { return m_Image; }

	private:
		void Invalidate();

	private:
		Image2DInfo m_CreateInfo;

		VkImage m_Image = VK_NULL_HANDLE;
		VmaAllocation m_Allocation;
		EImageState m_State;

		friend class RenderTexture;
	};

}
