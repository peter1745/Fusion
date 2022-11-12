#pragma once

namespace Fusion {

	enum class ERendererAPI { None = -1, D3D11, D3D12 };

	struct RenderSettings
	{
		ERendererAPI API = ERendererAPI::D3D11;

		static RenderSettings& Get();
	};

}
