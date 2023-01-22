#pragma once

#include <Fusion/Renderer/Camera.hpp>
#include <Fusion/World/World.hpp>

namespace FusionEditor {

	class WorldRenderer
	{
	public:
		// TODO(Peter): The view matrix should eventually either be part of Camera or passed somewhere else.
		//				The reason I haven't put the view matrix in Camera is because in play mode it's just 
		//				the inverse of the camera actor's transform
		virtual void Begin(const Fusion::Camera& InCamera, const glm::mat4& InViewMatrix) = 0;
		virtual void Render() = 0;
		virtual void End() = 0;
	};

}
