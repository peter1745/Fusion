#include "FusionPCH.hpp"
#include "Image.hpp"
#include "RenderSettings.hpp"

namespace Fusion {

	Shared<Image2D> Image2D::Create(const Image2DInfo& InCreateInfo)
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::Vulkan: return nullptr;
		}

		return nullptr;
	}

}
