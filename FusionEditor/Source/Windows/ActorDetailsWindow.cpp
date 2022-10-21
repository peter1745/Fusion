#include "ActorDetailsWindow.h"
#include "UI/ComponentRenderer.h"

#include "WindowManager.h"
#include "WorldOutlinerWindow.h"

#include "Fusion/World/World.h"
#include "Fusion/World/Components/AllComponents.h"

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

		ImGui::EndPopup();
	}

	void ActorDetailsWindow::OnSelectionChanged(Fusion::Shared<Fusion::Actor> InActor)
	{
		m_CurrentActor = InActor;
	}

}
