#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace Fusion {

	enum class EProjectionType { PerspectiveProjection, OrthographicProjection };

	class Camera
	{
	public:
		Camera() = default;
		Camera(EProjectionType InProjectionType)
			: m_ProjectionType(InProjectionType) {}

		void SetProjectionMatrix(const glm::mat4& InProjectionMatrix) { m_ProjectionMatrix = InProjectionMatrix; }
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

	private:
		EProjectionType m_ProjectionType = EProjectionType::PerspectiveProjection;
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
	};

}
