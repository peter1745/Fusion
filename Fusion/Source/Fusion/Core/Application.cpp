#include "FusionPCH.h"
#include "Application.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include "Fusion/Renderer/RenderData.h"

#include <glad/gl.h>
#include <glm/glm.hpp>

namespace Fusion {

	static Application* s_Application = nullptr;

	static Vertex s_TriangleVertices[] = {
		{ {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Top-Right
		{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // Bottom-Right
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // Bottom-Left
		{ { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } }  // Top-Left
	};

	static uint32_t s_TriangleIndices[] = { 0, 1, 2, 2, 3, 0 };

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

		m_VertexBuffer = Shared<OpenGLVertexBuffer>::Create(4 * sizeof(Vertex), s_TriangleVertices, VertexBufferLayout({
			{ 0, ShaderDataType::Float3, offsetof(Vertex, Position) },
			{ 1, ShaderDataType::Float4, offsetof(Vertex, Color) }
		}));

		m_IndexBuffer = Shared<OpenGLIndexBuffer>::Create(6, s_TriangleIndices);

		ShaderSpecification Spec;
		Spec.VertexFilePath = "Resources/Shaders/VertexShader.vert";
		Spec.FragmentFilePath = "Resources/Shaders/FragmentShader.frag";
		m_Shader = Shared<OpenGLShader>::Create(Spec);

		m_CommandBuffer = Shared<RenderCommandBuffer>::Create();
	}

	Application::~Application()
	{
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
			m_CommandBuffer->RecordCommand([]()
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glClearColor(0, 0, 0, 1);
			});

			m_CommandBuffer->RecordCommand([InVertexBuffer = m_VertexBuffer, InIndexBuffer = m_IndexBuffer, InShader = m_Shader]() mutable
			{
				InShader->Bind();
				InVertexBuffer->Bind();
				InIndexBuffer->Bind();
				glDrawElements(GL_TRIANGLES, InIndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
			});

			m_CommandBuffer->Execute();

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
