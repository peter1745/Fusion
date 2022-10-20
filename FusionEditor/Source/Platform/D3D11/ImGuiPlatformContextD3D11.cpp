#include "ImGuiPlatformContextD3D11.h"

#include <Fusion/Core/Application.h>

#include <GLFW/glfw3.h>
#include <d3d11.h>

#include <Platform/D3D11/D3D11Context.h>

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_dx11.h>

namespace FusionEditor {

	void ImGuiPlatformContextD3D11::InitPlatform(const Fusion::Unique<Fusion::Window>& InWindow, const Fusion::Shared<Fusion::GraphicsContext>& InContext)
	{
		Fusion::Shared<Fusion::D3D11Context> D3DContext = InContext.As<Fusion::D3D11Context>();
		ImGui_ImplGlfw_InitForOther(static_cast<GLFWwindow*>(InWindow->GetWindowHandle()), true);
		ImGui_ImplDX11_Init(D3DContext->GetDevice(), D3DContext->GetDeviceContext());
	}

	void ImGuiPlatformContextD3D11::BeginFramePlatform()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}

	void ImGuiPlatformContextD3D11::EndFramePlatform()
	{
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* ContextBackup = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(ContextBackup);
		}
	}

	void ImGuiPlatformContextD3D11::ShutdownPlatform()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}

}
