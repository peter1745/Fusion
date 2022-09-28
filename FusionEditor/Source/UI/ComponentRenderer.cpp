#include "ComponentRenderer.h"
#include "UILibrary.h"

namespace FusionEditor {

	void ComponentUI<Fusion::TransformComponent>::Render(Fusion::Shared<Fusion::Actor> InActor, Fusion::TransformComponent* InComp)
	{
		if (!UI::BeginHeader("Transform"))
			return;

		ImGui::DragFloat3("Translation", &InComp->Location.x);
		ImGui::DragFloat3("Scale", &InComp->Scale.x);

		glm::vec3 RotationDegrees = glm::degrees(glm::eulerAngles(InComp->Rotation));
		if (ImGui::DragFloat3("Rotation", &RotationDegrees.x))
			InComp->Rotation = glm::quat(glm::radians(RotationDegrees));

		UI::EndHeader();
	}

	void ComponentUI<Fusion::SpriteComponent>::Render(Fusion::Shared<Fusion::Actor> InActor, Fusion::SpriteComponent* InComp)
	{
		if (!UI::BeginComponentHeader("Sprite", InComp->IsActive))
			return;

		UI::EndHeader();
	}
	
	void ComponentUI<Fusion::CameraComponent>::Render(Fusion::Shared<Fusion::Actor> InActor, Fusion::CameraComponent* InComp)
	{
		if (!UI::BeginComponentHeader("Camera", InComp->IsActive))
			return;

		UI::Dropdown("Projection Type", &InComp->ProjectionType, { "Perspective", "Orthographic" });

		switch (InComp->ProjectionType)
		{
		case Fusion::EProjectionType::PerspectiveProjection:
			RenderPerspectiveParameters(InComp);
			break;
		case Fusion::EProjectionType::OrthographicProjection:
			RenderOrthographicParameters(InComp);
			break;
		}

		ImGui::DragFloat("Near Plane", &InComp->NearPlane);
		ImGui::DragFloat("Far Plane", &InComp->FarPlane);

		UI::EndHeader();
	}

	void ComponentUI<Fusion::CameraComponent>::RenderPerspectiveParameters(Fusion::CameraComponent* InComp)
	{
		ImGui::DragFloat("Vertical FOV", &InComp->VerticalFOV);
	}

	void ComponentUI<Fusion::CameraComponent>::RenderOrthographicParameters(Fusion::CameraComponent* InComp)
	{
		ImGui::DragFloat("Orthographic Size", &InComp->OrthographicSize);
	}

}
