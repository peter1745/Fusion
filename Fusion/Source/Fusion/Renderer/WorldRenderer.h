#pragma once

#include "Fusion/World/World.h"
#include "Fusion/Renderer/Camera.h"
#include "Fusion/Renderer/Shader.h"
#include "Fusion/Renderer/Renderer.h"

namespace Fusion {

	class WorldRenderer
	{
	public:
		WorldRenderer(World* InWorld);

		// TODO(Peter): The view matrix should eventually either be part of Camera or passed somewhere else.
		//				The reason I haven't put the view matrix in Camera is because in play mode it's just 
		//				the inverse of the camera actor's transform
		void Begin(const Camera& InCamera, const glm::mat4& InViewMatrix);
		void Render();
		void End();

	private:
		World* m_World = nullptr;
		Shared<Shader> m_PBRShader = nullptr;
		Shared<UniformBuffer> m_CameraDataBuffer = nullptr;
		Shared<Renderer> m_Renderer = nullptr;
	};

}
