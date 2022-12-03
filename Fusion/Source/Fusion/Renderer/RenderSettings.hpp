#pragma once

namespace Fusion {

	enum class ERendererAPI
	{
		None = -1,
#ifdef FUSION_PLATFORM_WINDOWS
		D3D11, D3D12,
#endif
		Vulkan
	};

	struct RenderSettings
	{
		ERendererAPI API = ERendererAPI::Vulkan;

		static RenderSettings& Get();
	};

}
