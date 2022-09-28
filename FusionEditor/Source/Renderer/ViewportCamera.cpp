#include "ViewportCamera.h"
#include "Fusion/Core/Application.h"

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace FusionEditor {

	using namespace Fusion;

	ViewportCamera::ViewportCamera(float InWidth, float InHeight)
		: Camera(EProjectionType::PerspectiveProjection)
	{
		SetProjectionMatrix(glm::perspectiveFov(70.0f, InWidth, InHeight, 0.1f, 1000.0f));
	}

	void ViewportCamera::OnUpdate(float InDeltaTime)
	{
		GLFWwindow* NativeWindow = static_cast<GLFWwindow*>(Fusion::Application::Get().GetWindow()->GetNativeWindow());

		int32_t keyState = glfwGetKey(NativeWindow, GLFW_KEY_W);
		if (keyState == GLFW_PRESS || keyState == GLFW_REPEAT)
			m_Location += glm::vec3(0.0f, 0.0f, -1.0f) * InDeltaTime;

		keyState = glfwGetKey(NativeWindow, GLFW_KEY_A);
		if (keyState == GLFW_PRESS || keyState == GLFW_REPEAT)
			m_Location += glm::vec3(-1.0f, 0.0f, 0.0f) * InDeltaTime;

		keyState = glfwGetKey(NativeWindow, GLFW_KEY_S);
		if (keyState == GLFW_PRESS || keyState == GLFW_REPEAT)
			m_Location += glm::vec3(0.0f, 0.0f, 1.0f) * InDeltaTime;

		keyState = glfwGetKey(NativeWindow, GLFW_KEY_D);
		if (keyState == GLFW_PRESS || keyState == GLFW_REPEAT)
			m_Location += glm::vec3(1.0f, 0.0f, 0.0f) * InDeltaTime;

		keyState = glfwGetKey(NativeWindow, GLFW_KEY_SPACE);
		if (keyState == GLFW_PRESS || keyState == GLFW_REPEAT)
			m_Location += glm::vec3(0.0f, 1.0f, 0.0f) * InDeltaTime;

		keyState = glfwGetKey(NativeWindow, GLFW_KEY_LEFT_SHIFT);
		if (keyState == GLFW_PRESS || keyState == GLFW_REPEAT)
			m_Location += glm::vec3(0.0f, -1.0f, 0.0f) * InDeltaTime;

		keyState = glfwGetKey(NativeWindow, GLFW_KEY_Q);
		if (keyState == GLFW_PRESS || keyState == GLFW_REPEAT)
			m_Rotation -= glm::vec3(0.0f, 90.0f, 0.0f) * InDeltaTime;

		keyState = glfwGetKey(NativeWindow, GLFW_KEY_E);
		if (keyState == GLFW_PRESS || keyState == GLFW_REPEAT)
			m_Rotation += glm::vec3(0.0f, 90.0f, 0.0f) * InDeltaTime;

		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Location) * glm::toMat4(glm::quat(glm::radians(m_Rotation)));
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

}
