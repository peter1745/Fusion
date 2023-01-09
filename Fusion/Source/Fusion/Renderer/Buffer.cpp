#include "FusionPCH.hpp"
#include "Buffer.hpp"

#include "RenderSettings.hpp"

#include "Platform/Vulkan/VulkanBuffer.hpp"

namespace Fusion {

	Shared<Buffer> Buffer::Create(const BufferInfo& InCreateInfo)
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::Vulkan: return Shared<VulkanBuffer>::Create(InCreateInfo);
		}

		return nullptr;
	}

}
