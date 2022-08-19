#include "FusionPCH.h"
#include "Application.h"
#include "Platform/Vulkan/VulkanRenderer.h"

#include <glm/glm.hpp>

namespace Fusion {

	static Application* s_Application = nullptr;
	static Shared<VulkanRenderer> s_VulkanRenderer = nullptr;

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

		s_VulkanRenderer = Shared<VulkanRenderer>::Create();
	}

	Application::~Application()
	{
		s_VulkanRenderer = nullptr;
		m_Window = nullptr;
	}

	void Application::Run()
	{
		m_Running = true;
		OnInit();
		while (m_Running)
		{
			m_Window->ProcessEvents();

			// Render
			s_VulkanRenderer->BeginDraw();
			s_VulkanRenderer->Draw(0.0f, 0.0f);
			s_VulkanRenderer->EndDraw();
			s_VulkanRenderer->Submit();

			// Layers?
			OnUpdate(m_TimeStep);

			TimePoint time = std::chrono::high_resolution_clock::now();
			m_FrameTime = std::chrono::duration_cast<std::chrono::duration<float>>(time - m_LastFrameTime).count();
			m_TimeStep = glm::min<float>(m_FrameTime, 0.0333f);
			m_LastFrameTime = time;

			// TODO(Peter): Handle with event
			m_Running = !m_Window->ShouldClose();
		}
		OnShutdown();
	}

	Application& Application::Get() { return *s_Application; }

}
