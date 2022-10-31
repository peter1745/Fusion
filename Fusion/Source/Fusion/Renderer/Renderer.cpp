#include "FusionPCH.hpp"
#include "Renderer.hpp"

#ifdef FUSION_PLATFORM_WINDOWS
	#include "Platform/D3D11/D3D11Renderer.hpp"
#endif

// #include "Platform/OpenGL/OpenGLRenderer.h"

namespace Fusion {

	Shared<Renderer> Renderer::Create(ERendererAPI InRendererAPI)
	{
		//s_CurrentAPI = InRendererAPI;

		switch (s_CurrentAPI)
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::D3D11: return Shared<D3D11Renderer>::Create();
		}

		return nullptr;
	}

}
