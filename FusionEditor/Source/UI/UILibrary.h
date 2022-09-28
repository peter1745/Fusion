#pragma once

#include "Fusion/STL/InitializerList.h"

#include <string_view>
#include <format>

#include <ImGui/imgui.h>

namespace FusionEditor::UI {

	static void ShiftCursorX(float InAmount) { ImGui::SetCursorPosX(ImGui::GetCursorPosX() + InAmount); }
	static void ShiftCursorY(float InAmount) { ImGui::SetCursorPosY(ImGui::GetCursorPosY() + InAmount); }
	static void ShiftCursor(float InAmountX, float InAmountY)
	{
		ShiftCursorX(InAmountX);
		ShiftCursorY(InAmountY);
	}

	static bool BeginHeader(std::string_view InLabel, bool InDefaultOpen = true)
	{
		ImGuiTreeNodeFlags TreeNodeFlags =
			ImGuiTreeNodeFlags_Framed
			| ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_AllowItemOverlap
			| ImGuiTreeNodeFlags_FramePadding;

		if (InDefaultOpen)
			TreeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

		ImGui::PushID(InLabel.data());
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 6.0f));
		const bool IsOpen = ImGui::TreeNodeEx("##HeaderTreeNode", TreeNodeFlags, "%s", InLabel.data());
		ImGui::PopStyleVar(2);
		ImGui::PopID();

		return IsOpen;
	}

	static bool BeginComponentHeader(std::string_view InLabel, bool& IsActive)
	{
		ImGuiTreeNodeFlags TreeNodeFlags =
			ImGuiTreeNodeFlags_Framed
			| ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_AllowItemOverlap
			| ImGuiTreeNodeFlags_FramePadding
			| ImGuiTreeNodeFlags_DefaultOpen;

		ImGui::PushID(InLabel.data());
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 6.0f));
		const bool IsOpen = ImGui::TreeNodeEx("##HeaderTreeNode", TreeNodeFlags, "%s", InLabel.data());

		// Component Active Checkbox
		{
			auto& Style = ImGui::GetStyle();
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(Style.FramePadding.x, Style.FramePadding.y - 2.0f));
			float OffsetX = ImGui::GetContentRegionAvail().x - Style.FramePadding.x;

			if (!IsOpen)
			{
				OffsetX = ImGui::GetContentRegionAvail().x - Style.FramePadding.x * 3.5f;
			}

			ImGui::SameLine(OffsetX);
			ShiftCursorY(2.0f);
			ImGui::Checkbox("##IsActive", &IsActive);
			ShiftCursorY(-2.0f);
			ImGui::PopStyleVar();
		}

		ImGui::PopStyleVar(2);
		ImGui::PopID();

		return IsOpen;
	}

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

	static void EndHeader()
	{
		ImGui::TreePop();
	}

}
