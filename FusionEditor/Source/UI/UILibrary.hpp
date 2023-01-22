#pragma once

#include "Fusion/World/Components/AllComponents.hpp"
#include "Fusion/Renderer/RenderTexture.hpp"

#include <FTL/InitializerList.hpp>

#include <string_view>
#include <spdlog/fmt/fmt.h>

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

namespace FusionEditor::UI {

	void ShiftCursorX(float InAmount);
	void ShiftCursorY(float InAmount);
	void ShiftCursor(float InAmountX, float InAmountY);

	bool BeginHeader(std::string_view InLabel, bool InDefaultOpen = true, bool* OutRightClicked = nullptr);
	bool BeginComponentHeader(std::string_view InLabel, bool& IsActive, bool* OutRightClicked = nullptr);
	void EndHeader();

	ImRect RectOffset(const ImRect& InRect, float InOffsetX, float InOffsetY);

	// MenuBar which allows you to specify its rectangle
	bool BeginMenuBar(const ImRect& InRect);
	void EndMenuBar();

	bool BeginPopup(std::string_view InLabel, float InMinWidth = 50.0f, ImGuiWindowFlags InFlags = 0);
	void EndPopup();

	// Uses Fusion's own mouse position, since ImGui's mouse position becomes invalid if the cursor is hidden or locked
	bool IsMouseHoveringRect(const ImVec2& InMin, const ImVec2& InMax, bool InClip = true);

	void BeginDisabled(bool InDisabled = true);
	void EndDisabled();

	template<typename TEnum>
	static bool Dropdown(std::string_view InLabel, TEnum* InSelectedValue, const FTL::InitializerList<std::string_view>& InOptionLabels)
	{
		size_t SelectedIndex = static_cast<size_t>(*InSelectedValue);
		std::string_view PreviewText = InOptionLabels[SelectedIndex];

		bool ValueChanged = false;

		if (ImGui::BeginCombo(InLabel.data(), PreviewText.data()))
		{
			for (size_t i = 0; i < InOptionLabels.Size(); i++)
			{
				std::string_view OptionLabel = InOptionLabels[i];
				if (ImGui::Selectable(OptionLabel.data(), SelectedIndex == i))
				{
					*InSelectedValue = static_cast<TEnum>(i);
					ValueChanged = true;
				}
			}

			ImGui::EndCombo();
		}

		return ValueChanged;
	}

	using UIEditableComponents = Fusion::ComponentGroup<
		Fusion::TransformComponent,
		Fusion::CameraComponent,
		Fusion::MeshComponent,
		Fusion::PhysicsBodyComponent,
		Fusion::SphereShapeComponent,
		Fusion::BoxShapeComponent
	>;

}
