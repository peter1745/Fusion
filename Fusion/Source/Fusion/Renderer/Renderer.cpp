#include "FusionPCH.h"
#include "Renderer.h"

#ifdef FUSION_PLATFORM_WINDOWS
	#include "Platform/D3D11/D3D11Renderer.h"	
#endif

// #include "Platform/OpenGL/OpenGLRenderer.h"

namespace Fusion {

	Shared<Renderer> Renderer::Create(ERendererAPI InRendererAPI)
	{
		s_CurrentAPI = InRendererAPI;

		switch (InRendererAPI)
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::D3D11: return Shared<D3D11Renderer>::Create();
		case ERendererAPI::OpenGL: return nullptr;
		}

		FUSION_CORE_VERIFY(false);
		return nullptr;
	}

}
