#pragma once

#include "Fusion/World/World.h"

namespace Fusion {

	class WorldRenderer
	{
	public:
		WorldRenderer(World* InWorld, uint32_t InFramebufferWidth, uint32_t InFramebufferHeight);

		void Begin();
		void Render();
		void End();

	private:
		World* m_World = nullptr;

	};

}
