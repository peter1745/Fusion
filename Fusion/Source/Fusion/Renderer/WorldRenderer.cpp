#include "FusionPCH.h"
#include "WorldRenderer.h"
#include "Renderer.h"

#include "Fusion/World/Components/AllComponents.h"

namespace Fusion {

	WorldRenderer::WorldRenderer(World* InWorld, uint32_t InFramebufferWidth, uint32_t InFramebufferHeight)
		: m_World(InWorld)
	{
	}

	void WorldRenderer::Begin()
	{
	}

	void WorldRenderer::Render()
	{
		/*const auto& SpriteActors = m_World->FindAllActorsWith<TransformComponent, SpriteComponent>();

		for (auto Actor : SpriteActors)
		{
			
		}*/
	}

	void WorldRenderer::End()
	{
	}

}
