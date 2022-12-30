#include "ImGuiPlatformContextD3D11.hpp"

#include <Fusion/Core/Application.hpp>

#include <GLFW/glfw3.h>
#include <d3d11.h>

#include <Platform/D3D11/D3D11Context.hpp>

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_dx11.h>

namespace FusionEditor {

	void ImGuiPlatformContextD3D11::InitPlatform(const Fusion::Unique<Fusion::Window>& InWindow, const Fusion::Shared<Fusion::GraphicsContext>& InContext, const Fusion::Shared<Fusion::SwapChain>& InSwapChain)
	{
		auto Device = InContext->GetDevice().As<Fusion::D3D11Device>();
		ImGui_ImplGlfw_InitForOther(static_cast<GLFWwindow*>(InWindow->GetWindowHandle()), true);
		ImGui_ImplDX11_Init(Device->GetDevice(), Device->GetDeviceContext());
	}

	void ImGuiPlatformContextD3D11::BeginFramePlatform()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}

	void ImGuiPlatformContextD3D11::EndFramePlatform(Fusion::CommandList* InCommandList)
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
