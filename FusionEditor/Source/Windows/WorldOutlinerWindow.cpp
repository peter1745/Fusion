#include "WorldOutlinerWindow.h"
#include "WindowManager.h"
#include "ActorDetailsWindow.h"

#include "Fusion/World/Components/AllComponents.h"

namespace FusionEditor {

	WorldOutlinerWindow::WorldOutlinerWindow(Fusion::World* InWorld)
		: EditorWindow("WorldOutlinerWindow"), m_World(InWorld)
	{
		SetTitle("World Outliner");
	}

	WorldOutlinerWindow::~WorldOutlinerWindow()
	{
		m_World = nullptr;
	}

	void WorldOutlinerWindow::RenderContents()
	{
		auto AllActors = m_World->FindAllActorsWith<Fusion::TransformComponent>();

		if (ImGui::BeginPopupContextWindow("WorldOutlinerContextWindow"))
		{
			if (ImGui::MenuItem("Create Actor"))
				m_World->CreateActor("New Actor");

			ImGui::EndPopup();
		}

		for (auto& Actor : AllActors)
		{
			if (ImGui::Button(Actor->Name.c_str()))
			{
				WindowManager::Get()->GetWindowOfType<ActorDetailsWindow>()->SetCurrentActor(Actor);
			}
		}
	}

}
