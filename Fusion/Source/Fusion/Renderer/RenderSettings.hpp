#pragma once

namespace Fusion {

	enum class ERendererAPI { None = -1, D3D11, D3D12 };

	struct RenderSettings
	{
		ERendererAPI API = ERendererAPI::D3D12;

		inline static RenderSettings& Get()
		{
			static RenderSettings s_Settings;
			return s_Settings;
		}
	};

}
