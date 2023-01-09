#include "FusionPCH.hpp"
#include "DescriptorHeap.hpp"
#include "RenderSettings.hpp"

namespace Fusion {

	Shared<DescriptorHeap> DescriptorHeap::Create(const DescriptorHeapInfo& InCreateInfo)
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::Vulkan: return nullptr;
		}

		return nullptr;
	}

}
