#include "FusionPCH.hpp"
#include "Texture.hpp"
#include "RenderSettings.hpp"

namespace Fusion {

	Shared<Texture2D> Texture2D::Create(const Texture2DInfo& InCreateInfo)
	{
		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::Vulkan: return nullptr;
		}

		return nullptr;
	}

}
