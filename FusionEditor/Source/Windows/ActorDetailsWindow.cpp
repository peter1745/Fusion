#include "ActorDetailsWindow.h"
#include "ComponentRenderers/ComponentRenderer.h"

#include "Fusion/World/World.h"
#include "Fusion/World/Components/AllComponents.h"

namespace FusionEditor {

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

		ImGui::Text(m_CurrentActor->Name.c_str());

		Fusion::ComponentUtils::All(m_CurrentActor, [InActor = m_CurrentActor](auto* InComp)
		{
			ComponentRenderer<std::remove_pointer_t<decltype(InComp)>>::Render(InActor, InComp);
		});
	}

}
