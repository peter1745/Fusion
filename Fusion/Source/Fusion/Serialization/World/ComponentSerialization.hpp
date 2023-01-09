#pragma once

#include "Fusion/World/Components/AllComponents.hpp"
#include "WorldWriter.hpp"
#include "WorldReader.hpp"

#include <string_view>

namespace Fusion {

	template<typename TComponent>
	struct ComponentSerializer {};

	template<>
	struct ComponentSerializer<ActorComponent>
	{
		inline static const std::string Name = "ActorComponent";

		static void Serialize(WorldYAMLWriter& InWriter, const ActorComponent* InComponent)
		{
			InWriter.Write("ID", InComponent->ID);
		}

		static void Deserialize(const WorldYAMLReader& InReader, ActorComponent& InComponent)
		{
			InComponent.ID = InReader.Read<ActorID>("ID");
		}
	};

	template<>
	struct ComponentSerializer<TransformComponent>
	{
		inline static const std::string Name = "TransformComponent";

		static void Serialize(WorldYAMLWriter& InWriter, const TransformComponent* InComponent)
		{
			InWriter.Write("Location", InComponent->Location);
			InWriter.Write("Rotation", InComponent->GetRotationEuler());
			InWriter.Write("Scale", InComponent->Scale);
		}

		static void Deserialize(const WorldYAMLReader& InReader, TransformComponent& InComponent)
		{
			InComponent.Location = InReader.Read<glm::vec3>("Location", { 0.0f, 0.0f, 0.0f });
			InComponent.SetRotationEuler(InReader.Read<glm::vec3>("Rotation", { 0.0f, 0.0f, 0.0f }));
			InComponent.Scale = InReader.Read<glm::vec3>("Scale", { 1.0f, 1.0f, 1.0f });
		}
	};

	template<>
	struct ComponentSerializer<RelationshipComponent>
	{
		inline static const std::string Name = "RelationshipComponent";

		static void Serialize([[maybe_unused]] WorldYAMLWriter& InWriter, [[maybe_unused]] const RelationshipComponent* InComponent)
		{
			InWriter.Write("FirstChildID", InComponent->FirstChildID);
			InWriter.Write("NextSiblingID", InComponent->NextSiblingID);
			InWriter.Write("PrevSiblingID", InComponent->PrevSiblingID);
			InWriter.Write("ParentID", InComponent->ParentID);
		}

		static void Deserialize([[maybe_unused]] const WorldYAMLReader& InReader, [[maybe_unused]] RelationshipComponent& InComponent)
		{
			InComponent.FirstChildID = InReader.Read<ActorID>("FirstChildID", ActorID(0));
			InComponent.NextSiblingID = InReader.Read<ActorID>("NextSiblingID", ActorID(0));
			InComponent.PrevSiblingID = InReader.Read<ActorID>("PrevSiblingID", ActorID(0));
			InComponent.ParentID = InReader.Read<ActorID>("ParentID", ActorID(0));
		}
	};

	template<>
	struct ComponentSerializer<MeshComponent>
	{
		inline static const std::string Name = "MeshComponent";

		static void Serialize([[maybe_unused]] WorldYAMLWriter& InWriter, [[maybe_unused]] const MeshComponent* InComponent)
		{
			InWriter.Write("MeshHandle", InComponent->MeshHandle);
		}

		static void Deserialize([[maybe_unused]] const WorldYAMLReader& InReader, [[maybe_unused]] MeshComponent& InComponent)
		{
			InComponent.MeshHandle = InReader.Read<AssetHandle>("MeshHandle", AssetHandle(0));
		}
	};

	template<>
	struct ComponentSerializer<CameraComponent>
	{
		inline static const std::string Name = "CameraComponent";

		static void Serialize([[maybe_unused]] WorldYAMLWriter& InWriter, [[maybe_unused]] const CameraComponent* InComponent)
		{
		}

		static void Deserialize([[maybe_unused]] const WorldYAMLReader& InReader, [[maybe_unused]] CameraComponent& InComponent)
		{
		}
	};

	template<>
	struct ComponentSerializer<PhysicsBodyComponent>
	{
		inline static const std::string Name = "PhysicsBodyComponent";

		static void Serialize([[maybe_unused]] WorldYAMLWriter& InWriter, [[maybe_unused]] const PhysicsBodyComponent* InComponent)
		{
			InWriter.Write("Mass", InComponent->Mass);
		}

		static void Deserialize([[maybe_unused]] const WorldYAMLReader& InReader, [[maybe_unused]] PhysicsBodyComponent& InComponent)
		{
			InComponent.Mass = InReader.Read<float>("Mass", 1.0f);
		}
	};

	template<>
	struct ComponentSerializer<SphereShapeComponent>
	{
		inline static const std::string Name = "SphereShapeComponent";

		static void Serialize(WorldYAMLWriter& InWriter, const SphereShapeComponent* InComponent)
		{
			InWriter.Write("Radius", InComponent->Radius);
		}

		static void Deserialize(const WorldYAMLReader& InReader, SphereShapeComponent& InComponent)
		{
			InComponent.Radius = InReader.Read<float>("Radius", 0.5f);
		}
	};

	namespace ComponentUtils {

		template<typename TFunc, typename... TComponents>
		inline static void FindByName(const std::string& InName, TFunc InFunc)
		{
			([&]()
			{
				if (ComponentSerializer<TComponents>::Name == InName)
				{
					TComponents NewComponent;
					InFunc(NewComponent);
				}
			}(), ...);
		}

		template<typename TFunc, typename... TComponents>
		inline static void FindByName(const std::string& InName, [[maybe_unused]] ComponentGroup<TComponents...> InCompGroup, TFunc InFunc)
		{
			FindByName<TFunc, TComponents...>(InName, InFunc);
		}
		
		template<typename TFunc>
		inline static void AllFindByName(const std::string& InName, TFunc InFunc)
		{
			FindByName<TFunc>(InName, AllComponents{}, InFunc);
		}

	}
}
