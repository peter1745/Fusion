#include "ImGuiPlatformContextD3D12.hpp"

#include <Fusion/Core/Application.hpp>

#include <GLFW/glfw3.h>

#include <Platform/D3D12/D3D12Context.hpp>

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_dx12.h>

namespace FusionEditor {

	void ImGuiPlatformContextD3D12::InitPlatform(const Fusion::Unique<Fusion::Window>& InWindow, const Fusion::Shared<Fusion::GraphicsContext>& InContext, const Fusion::Shared<Fusion::SwapChain>& InSwapChain)
	{
		Fusion::Shared<Fusion::D3D12Context> D3DContext = InContext.As<Fusion::D3D12Context>();

		auto Heap = InContext->GetDescriptorHeap(Fusion::EDescriptorHeapType::SRV_CBV_UAV).As<Fusion::D3D12DescriptorHeap>();
		auto HeapRef = Heap->Reserve();

		D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle = { Heap->GetCPUDescriptorHandle(HeapRef) };
		D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = { Heap->GetGPUDescriptorHandle(HeapRef) };

		uint32_t FramesInFlight = std::min(3U, InSwapChain->GetImageCount());

		ImGui_ImplGlfw_InitForOther(static_cast<GLFWwindow*>(InWindow->GetWindowHandle()), true);
		ImGui_ImplDX12_Init(
			D3DContext->GetDevice().As<Fusion::D3D12Device>()->GetDevice().Get(),
		    FramesInFlight,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			Heap->GetHeap(),
			CPUHandle,
			GPUHandle
		);
	}

	void ImGuiPlatformContextD3D12::BeginFramePlatform()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}

	void ImGuiPlatformContextD3D12::EndFramePlatform(Fusion::CommandList* InCommandList)
	{
		auto Context = Fusion::GraphicsContext::Get<Fusion::D3D12Context>();
		auto& CmdList = dynamic_cast<Fusion::D3D12CommandList*>(InCommandList)->GetNativeList();
		auto Heap = Context->GetDescriptorHeap(Fusion::EDescriptorHeapType::SRV_CBV_UAV);
		CmdList->SetDescriptorHeaps(1, Heap.As<Fusion::D3D12DescriptorHeap>()->GetHeap());

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
