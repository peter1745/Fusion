#pragma once

namespace Fusion {

	enum class ERendererAPI { None = -1, Vulkan };

	struct RenderSettings
	{
		ERendererAPI API = ERendererAPI::Vulkan;

		static RenderSettings& Get();
	};

}
