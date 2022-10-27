#include "ActorDetailsWindow.hpp"
#include "UI/ComponentRenderer.hpp"

#include "WindowManager.hpp"
#include "WorldOutlinerWindow.hpp"

#include "Fusion/World/World.hpp"
#include "Fusion/World/Components/AllComponents.hpp"

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

		ComponentUtils::All(m_CurrentActor, [InActor = m_CurrentActor]<typename TComponent>(TComponent* InComp)
		{
			ComponentUI<TComponent>::Render(InActor, InComp);
		});
	}

	void ActorDetailsWindow::RenderComponentMenu()
	{
		if (!ImGui::BeginPopupContextWindow("AddComponentPopupMenu"))
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
