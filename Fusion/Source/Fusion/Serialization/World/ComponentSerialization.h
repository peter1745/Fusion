#pragma once

#include "Fusion/World/Components/AllComponents.h"
#include "WorldWriter.h"

#include <string_view>

namespace Fusion {

	template<typename TComponent>
	struct ComponentSerializer {};

	template<>
	struct ComponentSerializer<TransformComponent>
	{
		inline static const std::string Name = "TransformComponent";

		static void Serialize(WorldYAMLWriter& InWriter, const TransformComponent* InComponent)
		{
			InWriter.Write("Location", InComponent->Location);
			InWriter.Write("Rotation", InComponent->Rotation);
			InWriter.Write("Scale", InComponent->Scale);
		}

		/*static void Deserialize(const WorldReader& InReader, TransformComponent& InComponent)
		{
			InReader.EnterComponent("Transform");
			InComponent.Location = InReader.Read<glm::vec3>("Location");
			InComponent.Rotation = InReader.Read<glm::quat>("Rotation");
			InComponent.Scale = InReader.Read<glm::vec3>("Scale");
			InReader.ExitComponent();
		}*/
	};

	template<>
	struct ComponentSerializer<MeshComponent>
	{
		inline static const std::string Name = "MeshComponent";

		static void Serialize(WorldYAMLWriter& InWriter, const MeshComponent* InComponent)
		{
		}

		/*static void Deserialize(const WorldReader& InReader, TransformComponent& InComponent)
		{
			InReader.EnterComponent("Transform");
			InComponent.Location = InReader.Read<glm::vec3>("Location");
			InComponent.Rotation = InReader.Read<glm::quat>("Rotation");
			InComponent.Scale = InReader.Read<glm::vec3>("Scale");
			InReader.ExitComponent();
		}*/
	};

	template<>
	struct ComponentSerializer<CameraComponent>
	{
		inline static const std::string Name = "CameraComponent";

		static void Serialize(WorldYAMLWriter& InWriter, const CameraComponent* InComponent)
		{
		}

		/*static void Deserialize(const WorldReader& InReader, TransformComponent& InComponent)
		{
			InReader.EnterComponent("Transform");
			InComponent.Location = InReader.Read<glm::vec3>("Location");
			InComponent.Rotation = InReader.Read<glm::quat>("Rotation");
			InComponent.Scale = InReader.Read<glm::vec3>("Scale");
			InReader.ExitComponent();
		}*/
	};

	template<>
	struct ComponentSerializer<SpriteComponent>
	{
		inline static const std::string Name = "SpriteComponent";

		static void Serialize(WorldYAMLWriter& InWriter, const SpriteComponent* InComponent)
		{
		}

		/*static void Deserialize(const WorldReader& InReader, TransformComponent& InComponent)
		{
			InReader.EnterComponent("Transform");
			InComponent.Location = InReader.Read<glm::vec3>("Location");
			InComponent.Rotation = InReader.Read<glm::quat>("Rotation");
			InComponent.Scale = InReader.Read<glm::vec3>("Scale");
			InReader.ExitComponent();
		}*/
	};

}
