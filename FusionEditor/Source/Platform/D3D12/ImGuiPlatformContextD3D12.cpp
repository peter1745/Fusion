#include "ImGuiPlatformContextD3D12.hpp"

#include <Fusion/Core/Application.hpp>

#include <GLFW/glfw3.h>

#include <Platform/D3D12/D3D12Context.hpp>

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_dx12.h>

namespace FusionEditor {

	void ImGuiPlatformContextD3D12::InitPlatform(const Fusion::Unique<Fusion::Window>& InWindow, const Fusion::Shared<Fusion::GraphicsContext>& InContext)
	{
		Fusion::Shared<Fusion::D3D12Context> D3DContext = InContext.As<Fusion::D3D12Context>();

		D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc = {};
		DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		DescriptorHeapDesc.NumDescriptors = 1;
		DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		D3DContext->GetDevice()->CreateDescriptorHeap(&DescriptorHeapDesc, m_ImGuiDescriptorHeap, m_ImGuiDescriptorHeap);

		ImGui_ImplGlfw_InitForOther(static_cast<GLFWwindow*>(InWindow->GetWindowHandle()), true);
		ImGui_ImplDX12_Init(
			D3DContext->GetDevice().Get(),
			D3DContext->GetFramesInFlight(),
			DXGI_FORMAT_R8G8B8A8_UNORM,
			m_ImGuiDescriptorHeap.Get(),
			m_ImGuiDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			m_ImGuiDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
		);
	}

	void ImGuiPlatformContextD3D12::BeginFramePlatform()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}

	void ImGuiPlatformContextD3D12::EndFramePlatform()
	{
		auto Context = Fusion::GraphicsContext::Get<Fusion::D3D12Context>();
		auto& CmdList = static_cast<Fusion::D3D12CommandList*>(Context->GetCurrentCommandList())->GetNativeList();
		CmdList->SetDescriptorHeaps(1, m_ImGuiDescriptorHeap);

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), CmdList);

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* ContextBackup = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(ContextBackup);
		}
	}

	void ImGuiPlatformContextD3D12::ShutdownPlatform()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}

}
