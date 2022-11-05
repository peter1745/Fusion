#pragma once

#include "Fusion/World/World.hpp"
#include "Fusion/Renderer/Camera.hpp"
#include "Fusion/Renderer/Shader.hpp"
#include "Fusion/Renderer/Renderer.hpp"
#include "Fusion/Renderer/DescriptorHeap.hpp"

namespace Fusion {

	class WorldRenderer
	{
	public:
		WorldRenderer(DescriptorHeap* InDescriptorHeap, const Shared<World>& InWorld);

		// TODO(Peter): The view matrix should eventually either be part of Camera or passed somewhere else.
		//				The reason I haven't put the view matrix in Camera is because in play mode it's just 
		//				the inverse of the camera actor's transform
		void Begin(const Camera& InCamera, const glm::mat4& InViewMatrix);
		void Render();
		void End();

	private:
		struct TransformData
		{
			glm::mat4 ViewProjection;
			glm::mat4 Transform;
			uint64_t ActorID;
		} m_TransformData;

	private:
		Shared<World> m_World = nullptr;
		Shared<Shader> m_PBRShader = nullptr;
		Shared<UniformBuffer> m_TransformBuffer = nullptr;
		Shared<Renderer> m_Renderer = nullptr;
	};

}
