#pragma once

#include <Fusion/Renderer/Camera.h>

namespace FusionEditor {

	class ViewportCamera : public Fusion::Camera
	{
	public:
		ViewportCamera(float InWidth, float InHeight);

		void OnUpdate(float InDeltaTime);

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

	private:
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::vec3 m_Location = glm::vec3(0.0f);
		glm::vec3 m_Rotation = glm::vec3(0.0f);
	};

}
