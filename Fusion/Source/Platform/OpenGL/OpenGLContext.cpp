#include "FusionPCH.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/gl.h>

namespace Fusion {

	void APIENTRY OpenGLDebugHandler(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
	{
		// ignore non-significant error/warning codes
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

		FUSION_CORE_ERROR("---------------");
		FUSION_CORE_ERROR("OpenGL Message ({}): {}", id, message);

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:             FUSION_CORE_ERROR("Source: API"); break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   FUSION_CORE_ERROR("Source: Window System"); break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: FUSION_CORE_ERROR("Source: Shader Compiler"); break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     FUSION_CORE_ERROR("Source: Third Party"); break;
		case GL_DEBUG_SOURCE_APPLICATION:     FUSION_CORE_ERROR("Source: Application"); break;
		case GL_DEBUG_SOURCE_OTHER:           FUSION_CORE_ERROR("Source: Other"); break;
		}

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:               FUSION_CORE_ERROR("Type: Error"); break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: FUSION_CORE_ERROR("Type: Deprecated Behaviour"); break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  FUSION_CORE_ERROR("Type: Undefined Behaviour"); break;
		case GL_DEBUG_TYPE_PORTABILITY:         FUSION_CORE_ERROR("Type: Portability"); break;
		case GL_DEBUG_TYPE_PERFORMANCE:         FUSION_CORE_ERROR("Type: Performance"); break;
		case GL_DEBUG_TYPE_MARKER:              FUSION_CORE_ERROR("Type: Marker"); break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          FUSION_CORE_ERROR("Type: Push Group"); break;
		case GL_DEBUG_TYPE_POP_GROUP:           FUSION_CORE_ERROR("Type: Pop Group"); break;
		case GL_DEBUG_TYPE_OTHER:               FUSION_CORE_ERROR("Type: Other"); break;
		}

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         FUSION_CORE_ERROR("Severity: high"); break;
		case GL_DEBUG_SEVERITY_MEDIUM:       FUSION_CORE_ERROR("Severity: medium"); break;
		case GL_DEBUG_SEVERITY_LOW:          FUSION_CORE_ERROR("Severity: low"); break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: FUSION_CORE_ERROR("Severity: notification"); break;
		}

		FUSION_CORE_ERROR("---------------");
	}

	OpenGLContext::OpenGLContext(const Unique<Window>& InWindow)
	{
		glfwMakeContextCurrent(static_cast<GLFWwindow*>(InWindow->GetNativeWindow()));
		int status = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
		FUSION_CORE_VERIFY(status > 0);

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLDebugHandler, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

		glFrontFace(GL_CW);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	OpenGLContext::~OpenGLContext()
	{

	}

}
