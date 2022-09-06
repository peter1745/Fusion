#include "FusionPCH.h"
#include "Application.h"

#include "Fusion/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include <glad/gl.h>
#include <glm/glm.hpp>

namespace Fusion {

	static Application* s_Application = nullptr;

	Application::Application(const ApplicationSpecification& InSpecification)
		: m_Specification(InSpecification)
	{
		FUSION_CORE_VERIFY(s_Application == nullptr);
		s_Application = this;

		WindowSpecification WindowSpec = {};
		WindowSpec.Title = m_Specification.Title;
		WindowSpec.Width = m_Specification.WindowWidth;
		WindowSpec.Height = m_Specification.WindowHeight;
		m_Window = Window::Create(WindowSpec);
		m_Window->Init();

		m_GraphicsContext = Shared<OpenGLContext>::Create(m_Window);

		Renderer::Initialize();
	}

	Application::~Application()
	{
		Renderer::Shutdown();
		m_Window = nullptr;
	}

	void Application::Run()
	{
		m_Running = true;
		OnInit();
		while (m_Running)
		{
			m_Window->ProcessEvents();

			OnUpdate(m_TimeStep);

			TimePoint time = std::chrono::high_resolution_clock::now();
			m_FrameTime = std::chrono::duration_cast<std::chrono::duration<float>>(time - m_LastFrameTime).count();
			m_TimeStep = glm::min<float>(m_FrameTime, 0.0333f);
			m_LastFrameTime = time;

			m_Window->SwapBuffers();
			// TODO(Peter): Handle with event
			m_Running = !m_Window->ShouldClose();
		}
		OnShutdown();
	}

	Application& Application::Get() { return *s_Application; }

}
