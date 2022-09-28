#pragma once

#include "Fusion/World/World.h"
#include "Fusion/Renderer/Camera.h"
#include "Fusion/Renderer/Framebuffer.h"
#include "Fusion/Renderer/Shader.h"

namespace Fusion {

	class WorldRenderer
	{
	public:
		WorldRenderer(World* InWorld, uint32_t InFramebufferWidth, uint32_t InFramebufferHeight);

		// TODO(Peter): The view matrix should eventually either be part of Camera or passed somewhere else.
		//				The reason I haven't put the view matrix in Camera is because in play mode it's just 
		//				the inverse of the camera actor's transform
		void Begin(const Camera& InCamera, const glm::mat4& InViewMatrix);
		void Render();
		void End();

		Shared<Framebuffer> GetFinalImage() const { return m_Framebuffer; }

	private:
		World* m_World = nullptr;
		Shared<Framebuffer> m_Framebuffer = nullptr;
		Shared<Shader> m_PBRShader = nullptr;
	};

}
