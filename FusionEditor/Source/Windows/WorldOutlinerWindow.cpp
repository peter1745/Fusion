#include "WorldOutlinerWindow.hpp"
#include "WindowManager.hpp"

#include "Fusion/World/Components/AllComponents.hpp"

#include <ImGui/imgui_internal.h>

namespace FusionEditor {

	WorldOutlinerWindow::WorldOutlinerWindow(const Fusion::Shared<Fusion::World>& InWorld)
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
		auto AllActors = m_World->FindAllActorsWith<Fusion::RelationshipComponent>();

		if (ImGui::BeginPopupContextWindow("WorldOutlinerContextWindow"))
		{
			if (ImGui::MenuItem("Create Actor"))
				m_World->CreateActor("New Actor");

			ImGui::EndPopup();
		}

		for (auto& Actor : AllActors)
		{
			Fusion::RelationshipComponent* RelationshipComp = Actor->FindComponent<Fusion::RelationshipComponent>();

			if (RelationshipComp->ParentID != Fusion::ActorID::Invalid)
				continue;

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 8.0f));
			RenderActorNode(Actor);
			ImGui::PopStyleVar();
		}

		if (m_DeletedActor)
		{
			m_SelectionCallbacks.Invoke(nullptr);
			m_World->DestroyActor(m_DeletedActor->GetActorID());
			m_DeletedActor = nullptr;
		}
	}

	void WorldOutlinerWindow::RenderActorNode(const Fusion::Shared<Fusion::Actor>& InActor)
	{
		const bool WasOpen = ImGui::TreeNodeBehaviorIsOpen(ImGui::GetID(InActor->Name.c_str()));
		const bool IsOpen = ImGui::TreeNode(InActor->Name.c_str());

		if (ImGui::BeginPopupContextWindow("ActorContextMenu"))
		{
			if (ImGui::MenuItem("Delete"))
			{
				m_DeletedActor = InActor;
			}

			ImGui::EndPopup();
		}

		if (IsOpen != WasOpen)
			m_SelectionCallbacks.Invoke(InActor);

		if (!IsOpen)
			return;

		Fusion::RelationshipComponent* RelationshipComp = m_World->FindActorComponent<Fusion::RelationshipComponent>(InActor->GetActorID());
		Fusion::ActorID CurrentChildID = RelationshipComp->FirstChildID;

		while (CurrentChildID != Fusion::ActorID::Invalid)
		{
			auto ChildActor = m_World->FindActorWithID(CurrentChildID);
			Fusion::RelationshipComponent* ChildRelationshipComp = m_World->FindActorComponent<Fusion::RelationshipComponent>(ChildActor->GetActorID());

			RenderActorNode(ChildActor);

			CurrentChildID = ChildRelationshipComp->NextSiblingID;
		}

		ImGui::TreePop();
	}

}
