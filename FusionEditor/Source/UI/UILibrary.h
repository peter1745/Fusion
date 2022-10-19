#pragma once

#include "Fusion/STL/InitializerList.h"

#include <string_view>
#include <format>

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

namespace FusionEditor::UI {

	void ShiftCursorX(float InAmount);
	void ShiftCursorY(float InAmount);
	void ShiftCursor(float InAmountX, float InAmountY);

	bool BeginHeader(std::string_view InLabel, bool InDefaultOpen = true);
	bool BeginComponentHeader(std::string_view InLabel, bool& IsActive);
	void EndHeader();

	ImRect RectOffset(const ImRect& InRect, float InOffsetX, float InOffsetY);

	// MenuBar which allows you to specify its rectangle
	bool BeginMenuBar(const ImRect& InRect);
	void EndMenuBar();

	template<typename TEnum>
	static bool Dropdown(std::string_view InLabel, TEnum* InSelectedValue, const Fusion::InitializerList<std::string_view>& InOptionLabels)
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

}
