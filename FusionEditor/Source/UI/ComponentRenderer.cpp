#include "ComponentRenderer.hpp"
#include "UILibrary.hpp"

#include <Fusion/Core/Application.hpp>

namespace FusionEditor {

	void ComponentUI<Fusion::TransformComponent>::Render(Fusion::Shared<Fusion::Actor> InActor, Fusion::TransformComponent* InComp)
	{
		if (!UI::BeginHeader("Transform"))
			return;

		ImGui::DragFloat3("Translation", &InComp->Location.x, 0.1f);
		ImGui::DragFloat3("Scale", &InComp->Scale.x, 0.1f);

		glm::vec3 RotationDegrees = glm::degrees(InComp->GetRotationEuler());
		if (ImGui::DragFloat3("Rotation", &RotationDegrees.x, 0.1f))
			InComp->SetRotationEuler(glm::radians(RotationDegrees));

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

		float VerticalFOV = InComp->CameraInstance.GetVerticalFOV();
		if (ImGui::DragFloat("Vertical FOV", &VerticalFOV))
			InComp->CameraInstance.SetVerticalFOV(VerticalFOV);

		float NearPlane = InComp->CameraInstance.GetNearPlane();
		if (ImGui::DragFloat("Near Plane", &NearPlane))
			InComp->CameraInstance.SetNearPlane(NearPlane);
		
		float FarPlane = InComp->CameraInstance.GetFarPlane();
		if (ImGui::DragFloat("Far Plane", &FarPlane))
			InComp->CameraInstance.SetFarPlane(FarPlane);

		UI::EndHeader();
	}

	void ComponentUI<Fusion::MeshComponent>::Render(Fusion::Shared<Fusion::Actor> InActor, Fusion::MeshComponent* InComp)
	{
		if (!UI::BeginComponentHeader("Mesh", InComp->IsActive))
			return;

		if (!InComp->MeshHandle.IsValid())
		{
			ImGui::Button("Null");
		}
		else
		{
			const auto& Databank = Fusion::Application::Get().GetAssetStorage()->GetDatabank(InComp->MeshHandle);
			std::string Name = Databank.FilePath.filename().string();
			ImGui::Button(Name.c_str());
		}

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("AssetPayload");
			if (Payload)
				InComp->MeshHandle = *static_cast<Fusion::AssetHandle*>(Payload->Data);

			ImGui::EndDragDropTarget();
		}

		UI::EndHeader();
	}

}
