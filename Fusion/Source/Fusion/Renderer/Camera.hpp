#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace Fusion {

	class Camera
	{
	public:
		void SetProjectionMatrix(const glm::mat4& InProjectionMatrix) { m_ProjectionMatrix = InProjectionMatrix; }
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

	private:
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
	};

}
