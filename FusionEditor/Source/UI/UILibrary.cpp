#include "UILibrary.h"

#include <Fusion/IO/Mouse.h>

namespace FusionEditor::UI {

	void ShiftCursorX(float InAmount) { ImGui::SetCursorPosX(ImGui::GetCursorPosX() + InAmount); }
	void ShiftCursorY(float InAmount) { ImGui::SetCursorPosY(ImGui::GetCursorPosY() + InAmount); }
	void ShiftCursor(float InAmountX, float InAmountY)
	{
		ShiftCursorX(InAmountX);
		ShiftCursorY(InAmountY);
	}

	bool BeginHeader(std::string_view InLabel, bool InDefaultOpen)
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

	bool BeginComponentHeader(std::string_view InLabel, bool& IsActive)
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

	void EndHeader() { ImGui::TreePop(); }

	ImRect RectOffset(const ImRect& InRect, float InOffsetX, float InOffsetY)
	{
		ImRect result = InRect;
		result.Min.x += InOffsetX;
		result.Min.y += InOffsetY;
		result.Max.x += InOffsetX;
		result.Max.y += InOffsetY;
		return result;
	}

	bool BeginMenuBar(const ImRect& InRect)
	{
		ImGuiWindow* CurrentWindow = ImGui::GetCurrentWindow();
		if (CurrentWindow->SkipItems)
			return false;

		IM_ASSERT(!CurrentWindow->DC.MenuBarAppending);
		ImGui::BeginGroup();
		ImGui::PushID("##MenuBar");

		const ImVec2 Padding = CurrentWindow->WindowPadding;

		ImRect BarRect = UI::RectOffset(InRect, 0.0f, Padding.y);
		ImRect ClipRect(IM_ROUND(ImMax(CurrentWindow->Pos.x, BarRect.Min.x + CurrentWindow->WindowBorderSize + CurrentWindow->Pos.x - 10.0f)), IM_ROUND(BarRect.Min.y + CurrentWindow->WindowBorderSize + CurrentWindow->Pos.y),
			IM_ROUND(ImMax(BarRect.Min.x + CurrentWindow->Pos.x, BarRect.Max.x - ImMax(CurrentWindow->WindowRounding, CurrentWindow->WindowBorderSize))), IM_ROUND(BarRect.Max.y + CurrentWindow->Pos.y));

		ClipRect.ClipWith(CurrentWindow->OuterRectClipped);
		ImGui::PushClipRect(ClipRect.Min, ClipRect.Max, false);

		CurrentWindow->DC.CursorPos = CurrentWindow->DC.CursorMaxPos = ImVec2(BarRect.Min.x + CurrentWindow->Pos.x, BarRect.Min.y + CurrentWindow->Pos.y);
		CurrentWindow->DC.LayoutType = ImGuiLayoutType_Horizontal;
		CurrentWindow->DC.NavLayerCurrent = ImGuiNavLayer_Menu;
		CurrentWindow->DC.MenuBarAppending = true;
		ImGui::AlignTextToFramePadding();

		return true;
	}

	void EndMenuBar()
	{
		ImGuiWindow* CurrentWindow = ImGui::GetCurrentWindow();
		if (CurrentWindow->SkipItems)
			return;

		// Nav: When a move request within one of our child menu failed, capture the request to navigate among our siblings.
		if (ImGui::NavMoveRequestButNoResultYet() && (GImGui->NavMoveDir == ImGuiDir_Left || GImGui->NavMoveDir == ImGuiDir_Right) && (GImGui->NavWindow->Flags & ImGuiWindowFlags_ChildMenu))
		{
			// Try to find out if the request is for one of our child menu
			ImGuiWindow* NavEarliestChild = GImGui->NavWindow;
			while (NavEarliestChild->ParentWindow && (NavEarliestChild->ParentWindow->Flags & ImGuiWindowFlags_ChildMenu))
				NavEarliestChild = NavEarliestChild->ParentWindow;

			if (NavEarliestChild->ParentWindow == CurrentWindow && NavEarliestChild->DC.ParentLayoutType == ImGuiLayoutType_Horizontal && (GImGui->NavMoveFlags & ImGuiNavMoveFlags_Forwarded) == 0)
			{
				// To do so we claim focus back, restore NavId and then process the movement request for yet another frame.
				// This involve a one-frame delay which isn't very problematic in this situation. We could remove it by scoring in advance for multiple window (probably not worth bothering)
				const ImGuiNavLayer NavLayer = ImGuiNavLayer_Menu;
				IM_ASSERT(CurrentWindow->DC.NavLayersActiveMaskNext & (1 << NavLayer)); // Sanity check

				ImGui::FocusWindow(CurrentWindow);
				ImGui::SetNavID(CurrentWindow->NavLastIds[NavLayer], NavLayer, 0, CurrentWindow->NavRectRel[NavLayer]);
				
				GImGui->NavDisableHighlight = true; // Hide highlight for the current frame so we don't see the intermediary selection.
				GImGui->NavDisableMouseHover = GImGui->NavMousePosDirty = true;

				ImGui::NavMoveRequestForward(GImGui->NavMoveDir, GImGui->NavMoveClipDir, GImGui->NavMoveFlags, GImGui->NavMoveScrollFlags); // Repeat
			}
		}

		IM_MSVC_WARNING_SUPPRESS(6011);
		IM_ASSERT(CurrentWindow->DC.MenuBarAppending);

		ImGui::PopClipRect();
		ImGui::PopID();

		CurrentWindow->DC.MenuBarOffset.x = CurrentWindow->DC.CursorPos.x - CurrentWindow->Pos.x; // Save horizontal position so next append can reuse it. This is kinda equivalent to a per-layer CursorPos.
		GImGui->GroupStack.back().EmitItem = false;

		ImGui::EndGroup(); // Restore position on layer 0

		CurrentWindow->DC.LayoutType = ImGuiLayoutType_Vertical;
		CurrentWindow->DC.NavLayerCurrent = ImGuiNavLayer_Main;
		CurrentWindow->DC.MenuBarAppending = false;
	}

	bool IsMouseHoveringRect(const ImVec2& InMin, const ImVec2& InMax, bool InClip /*= true*/)
	{
		ImRect ClippedRect(InMin, InMax);
		if (InClip)
			ClippedRect.ClipWith(GImGui->CurrentWindow->ClipRect);

		const ImVec2 Min(ClippedRect.Min.x - GImGui->Style.TouchExtraPadding.x, ClippedRect.Min.y - GImGui->Style.TouchExtraPadding.y);
		const ImVec2 Max(ClippedRect.Max.x + GImGui->Style.TouchExtraPadding.x, ClippedRect.Max.y + GImGui->Style.TouchExtraPadding.y);
		const ImRect RectForTouch(Min, Max);
		const glm::vec2& MousePos = Fusion::Mouse::Get().GetPosition();
		if (!RectForTouch.Contains(ImVec2(MousePos.x, MousePos.y)))
			return false;

		if (!GImGui->MouseViewport->GetMainRect().Overlaps(ClippedRect))
			return false;

		return true;
	}

	void BeginDisabled(bool InDisabled)
	{
		if (InDisabled)
			ImGui::BeginDisabled(true);
	}

	void EndDisabled()
	{
		if (GImGui->DisabledStackSize > 0)
			ImGui::EndDisabled();
	}

}
