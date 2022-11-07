#include "ImGuiPlatformContext.hpp"

#include <Fusion/Renderer/RenderSettings.hpp>

#ifdef FUSION_PLATFORM_WINDOWS
	#include "Platform/D3D11/ImGuiPlatformContextD3D11.hpp"
	#include "Platform/D3D12/ImGuiPlatformContextD3D12.hpp"
#endif

#include <ImGui/imgui.h>

#include "ImGuizmo.h"

namespace FusionEditor {

	void ImGuiPlatformContext::Init(const Fusion::Unique<Fusion::Window>& InWindow, const Fusion::Shared<Fusion::GraphicsContext>& InContext)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		auto& IO = ImGui::GetIO();
		IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		IO.FontDefault = IO.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto-Regular.ttf", 16.0f);

		IO.ConfigWindowsMoveFromTitleBarOnly = true;

		InitStyle();
		InitPlatform(InWindow, InContext);
	}

	void ImGuiPlatformContext::BeginFrame()
	{
		BeginFramePlatform();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiPlatformContext::EndFrame()
	{
		EndFramePlatform();
	}

	void ImGuiPlatformContext::Shutdown()
	{
		ShutdownPlatform();
		ImGui::DestroyContext();
	}

	std::unique_ptr<ImGuiPlatformContext> ImGuiPlatformContext::Create()
	{
		switch (Fusion::RenderSettings::Get().API)
		{
		case Fusion::ERendererAPI::None: return nullptr;
		case Fusion::ERendererAPI::D3D11: return std::make_unique<ImGuiPlatformContextD3D11>();
		case Fusion::ERendererAPI::D3D12: return std::make_unique<ImGuiPlatformContextD3D12>();
		}

		return nullptr;
	}

	void ImGuiPlatformContext::InitStyle()
	{
		auto& UIStyle = ImGui::GetStyle();
		UIStyle.WindowPadding = { 2.0f, 2.0f };
		UIStyle.WindowBorderSize = 0.0f;
		UIStyle.WindowTitleAlign = { 0.5f, 0.5f };
		UIStyle.WindowRounding = 2.0f;
		UIStyle.DisplaySafeAreaPadding = { 0.0f, 6.0f };
		UIStyle.WindowMenuButtonPosition = ImGuiDir_None;

		UIStyle.Colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.0f);
		UIStyle.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.09f, 0.09f, 0.09f, 1.0f);
		UIStyle.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		UIStyle.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.18f, 0.18f, 0.19f, 1.0f);
		UIStyle.Colors[ImGuiCol_HeaderActive] = ImVec4(0.82f, 0.75f, 0.53f, 1.0f);

		UIStyle.Colors[ImGuiCol_Tab] = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
		UIStyle.Colors[ImGuiCol_TabActive] = ImVec4(0.2f, 0.2f, 0.19f, 1.0f);
		UIStyle.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.09f, 0.09f, 0.08f, 1.0f);
		UIStyle.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.13f, 1.0f);
		UIStyle.Colors[ImGuiCol_TabHovered] = ImVec4(0.22f, 0.37f, 1.0f, 0.58f);
		UIStyle.TabRounding = 2.0f;

		UIStyle.Colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.0f);
		UIStyle.PopupRounding = 2.0f;

		UIStyle.FramePadding = { 16.0f, 8.0f };
		UIStyle.FrameRounding = 2.0f;
		UIStyle.FrameBorderSize = 0.0f;

		UIStyle.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);

		UIStyle.ItemInnerSpacing = { 6.0f, 4.0f };
		UIStyle.IndentSpacing = 16.0f;

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			UIStyle.WindowRounding = 0.0f;
			UIStyle.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		
		// ImGuizmo Style
		auto& GizmoStyle = ImGuizmo::GetStyle();
		GizmoStyle.TranslationLineThickness = 8.0f;
		GizmoStyle.TranslationLineArrowSize = 10.0f;
		GizmoStyle.RotationLineThickness = 8.0f;
		GizmoStyle.RotationOuterLineThickness = 8.0f;
		GizmoStyle.ScaleLineThickness = 8.0f;
		GizmoStyle.ScaleLineCircleSize = 10.0f;
		GizmoStyle.HatchedAxisLineThickness = 0.0f;

		GizmoStyle.Colors[ImGuizmo::DIRECTION_X] = ImVec4(0.666f, 0.000f, 0.000f, 1.000f);
		GizmoStyle.Colors[ImGuizmo::DIRECTION_Y] = ImVec4(0.000f, 0.666f, 0.000f, 1.000f);
		GizmoStyle.Colors[ImGuizmo::DIRECTION_Z] = ImVec4(0.000f, 0.000f, 0.666f, 1.000f);
		GizmoStyle.Colors[ImGuizmo::PLANE_X] = ImVec4(0.666f, 0.000f, 0.000f, 0.75f);
		GizmoStyle.Colors[ImGuizmo::PLANE_Y] = ImVec4(0.000f, 0.666f, 0.000f, 0.75f);
		GizmoStyle.Colors[ImGuizmo::PLANE_Z] = ImVec4(0.000f, 0.000f, 0.666f, 0.75f);
		GizmoStyle.Colors[ImGuizmo::SELECTION] = ImVec4(1.000f, 0.7500f, 0.062f, 0.741f);
		GizmoStyle.Colors[ImGuizmo::INACTIVE] = ImVec4(0.600f, 0.600f, 0.600f, 0.600f);
		GizmoStyle.Colors[ImGuizmo::TRANSLATION_LINE] = ImVec4(0.666f, 0.666f, 0.666f, 0.666f);
		GizmoStyle.Colors[ImGuizmo::SCALE_LINE] = ImVec4(0.250f, 0.250f, 0.250f, 1.000f);
		GizmoStyle.Colors[ImGuizmo::ROTATION_USING_BORDER] = ImVec4(1.000f, 0.500f, 0.062f, 1.000f);
		GizmoStyle.Colors[ImGuizmo::ROTATION_USING_FILL] = ImVec4(1.000f, 0.500f, 0.062f, 0.500f);
		GizmoStyle.Colors[ImGuizmo::TEXT] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
		GizmoStyle.Colors[ImGuizmo::TEXT_SHADOW] = ImVec4(0.000f, 0.000f, 0.000f, 1.000f);
	}

}
