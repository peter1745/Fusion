#include "ActorDetailsWindow.hpp"
#include "UI/ComponentRenderer.hpp"
#include "UI/UILibrary.hpp"

#include "WindowManager.hpp"
#include "WorldOutlinerWindow.hpp"

#include "Fusion/World/World.hpp"
#include "Fusion/World/Components/AllComponents.hpp"
#include "Fusion/World/Components/ComponentAttributes.hpp"

namespace FusionEditor {

	using namespace Fusion;

	ActorDetailsWindow::ActorDetailsWindow(const ActorSelectionManager& InSelectionManager)
	    : EditorWindow("ActorDetailsWindowID"), m_SelectionManager(InSelectionManager)
	{
		SetTitle("Actor Details");

		m_SelectionManager->AddSelectionCallback(FUSION_BIND_FUNC(ActorDetailsWindow::OnActorSelected));
		m_SelectionManager->AddDeselectionCallback(FUSION_BIND_FUNC(ActorDetailsWindow::OnActorDeselected));
	}

	ActorDetailsWindow::~ActorDetailsWindow()
	{
		m_CurrentActor = nullptr;
	}

	void ActorDetailsWindow::RenderContents()
	{
		if (!m_CurrentActor)
			return;

		RenderComponentMenu();

		ComponentUtils::Each(m_CurrentActor, UI::UIEditableComponents {}, [InActor = m_CurrentActor]<typename TComponent>(TComponent* InComp) mutable {
			bool ShouldRender = true;
			bool OpenContextMenu = false;

			ImGui::PushID(static_cast<void*>(InComp));

			if constexpr (Fusion::ComponentAttributes<TComponent>::CanDisable)
				ShouldRender = UI::BeginComponentHeader(ComponentUI<TComponent>::DisplayName, InComp->IsActive, &OpenContextMenu);
			else
				ShouldRender = UI::BeginHeader(ComponentUI<TComponent>::DisplayName, true, &OpenContextMenu);

			if (OpenContextMenu)
				ImGui::OpenPopup("ComponentContextMenu");

			bool Removed = false;

			if (ShouldRender)
			{
				ComponentUI<TComponent>::Render(InActor, InComp);

				if (UI::BeginPopup("ComponentContextMenu", 150.0f))
				{
					if constexpr (Fusion::ComponentAttributes<TComponent>::Removable)
					{
						Removed = ImGui::MenuItem("Remove");
					}

					UI::EndPopup();
				}

				UI::EndHeader();
			}

			ImGui::PopID();

			if (Removed)
				InActor->RemoveComponent<TComponent>();
		});
	}

	void ActorDetailsWindow::RenderComponentMenu()
	{
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponentPopupMenu");

		if (!ImGui::BeginPopup("AddComponentPopupMenu"))
			return;

		RenderComponentMenuItem<CameraComponent>("Camera");
		RenderComponentMenuItem<MeshComponent>("Mesh");
		RenderComponentMenuItem<PhysicsBodyComponent>("Physics Body");
		RenderComponentMenuItem<SphereShapeComponent>("Sphere Shape");

		ImGui::EndPopup();
	}

	void ActorDetailsWindow::OnActorSelected(Fusion::Shared<Fusion::Actor> InActor)
	{
		m_CurrentActor = InActor;
	}

	void ActorDetailsWindow::OnActorDeselected(Fusion::Shared<Fusion::Actor> InActor)
	{
		if (m_CurrentActor == nullptr)
			return;

		if (m_CurrentActor != InActor)
			return;

		m_CurrentActor = nullptr;
	}

}
