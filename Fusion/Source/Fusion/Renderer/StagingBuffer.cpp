#include "FusionPCH.hpp"
#include "StagingBuffer.hpp"
#include "RenderSettings.hpp"

namespace Fusion {

	Unique<StagingBuffer> StagingBuffer::Create(const StagingBufferInfo& InCreateInfo)
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::Vulkan: return nullptr;
		}

		return nullptr;
	}

}
