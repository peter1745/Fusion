#include "ActorDetailsWindow.h"
#include "UI/ComponentRenderer.h"

#include "Fusion/World/World.h"
#include "Fusion/World/Components/AllComponents.h"

namespace FusionEditor {

	using namespace Fusion;

	ActorDetailsWindow::ActorDetailsWindow()
		: EditorWindow("ActorDetailsWindowID")
	{
		SetTitle("Actor Details");
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

}
