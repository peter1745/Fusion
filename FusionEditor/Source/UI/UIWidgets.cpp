#include "UIWidgets.hpp"

#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

#include <spdlog/fmt/fmt.h>

namespace FusionEditor::UI {

	bool InputText(std::string_view InLabel, std::string& OutString)
	{
		ImGui::Columns(2);
		ImGui::TextUnformatted(InLabel.data(), InLabel.data() + InLabel.size());
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(-1);

		std::string ID = fmt::format("##{}", InLabel);
		bool Result = ImGui::InputText(ID.c_str(), &OutString);
		ImGui::NextColumn();
		return Result;
	}

}
