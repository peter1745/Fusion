#include "AssetListWindow.hpp"

#include <Fusion/Core/Application.hpp>

#include <ImGui/misc/cpp/imgui_stdlib.h>

namespace FusionEditor {

	void AssetListWindow::RenderContents()
	{
		auto Storage = Fusion::Application::Get().GetAssetStorage();
		const auto& Databanks = Storage->GetDatabanks();

		for (const auto& [Handle, Databank] : Databanks)
		{
			std::string FilePath = Databank.FilePath.string();
			std::string ID = fmt::format("##{}", FilePath);
			ImGui::InputText(ID.c_str(), &FilePath, ImGuiInputTextFlags_ReadOnly);

			ImGui::SameLine();

			std::string ButtonText = fmt::format("Copy Handle##{}", FilePath);

			if (ImGui::Button(ButtonText.c_str()))
			{
				std::string HandleText = fmt::format("{}", (const uint64_t)Handle);
				ImGui::SetClipboardText(HandleText.c_str());
			}
		}
	}

}
