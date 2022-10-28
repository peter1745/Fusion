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

	ActorDetailsWindow::ActorDetailsWindow()
		: EditorWindow("ActorDetailsWindowID")
	{
		SetTitle("Actor Details");

		auto WorldOutliner = WindowManager::Get()->GetWindowOfType<WorldOutlinerWindow>();
		WorldOutliner->GetSelectionCallbackList().AddFunction(FUSION_BIND_FUNC(ActorDetailsWindow::OnSelectionChanged));
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

		ComponentUtils::All(m_CurrentActor, [InActor = m_CurrentActor]<typename TComponent>(TComponent* InComp) mutable
		{
			bool ShouldRender = true;
			bool OpenContextMenu = false;

			ImGui::PushID(static_cast<void*>(InComp));

			if constexpr (Fusion::ComponentAttributes<TComponent>::CanDisable)
				ShouldRender = UI::BeginComponentHeader(ComponentUI<TComponent>::DisplayName, InComp->IsActive, &OpenContextMenu);
			else
				ShouldRender = UI::BeginHeader(ComponentUI<TComponent>::DisplayName, &OpenContextMenu);

			if (OpenContextMenu)
				ImGui::OpenPopup("ComponentContextMenu");

			if (ShouldRender)
			{
				bool Removed = false;

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
				ImGui::PopID();

				if (Removed)
					InActor->RemoveComponent<TComponent>();
			}
		});
	}

	void ActorDetailsWindow::RenderComponentMenu()
	{
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponentPopupMenu");

		if (!ImGui::BeginPopup("AddComponentPopupMenu"))
			return;

		RenderComponentMenuItem<SpriteComponent>("Sprite");
		RenderComponentMenuItem<CameraComponent>("Camera");
		RenderComponentMenuItem<MeshComponent>("Mesh");

		ImGui::EndPopup();
	}

	void ActorDetailsWindow::OnSelectionChanged(Fusion::Shared<Fusion::Actor> InActor)
	{
		m_CurrentActor = InActor;
	}

}
