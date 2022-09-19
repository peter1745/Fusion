#include "ComponentRenderer.h"

#include <ImGui/imgui.h>

namespace FusionEditor {

	void ComponentRenderer<Fusion::TransformComponent>::Render(Fusion::Shared<Fusion::Actor> InActor, Fusion::TransformComponent* InComp)
	{
		ImGui::DragFloat3("Translation", &InComp->Location.x);
	}

}
