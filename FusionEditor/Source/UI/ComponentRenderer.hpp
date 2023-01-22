#pragma once

#include "Fusion/World/Actor.hpp"
#include "Fusion/World/Components/CoreComponents.hpp"

namespace FusionEditor {

	template<typename TComponent>
	class ComponentUI {};

	template<>
	class ComponentUI<Fusion::TransformComponent>
	{
	public:
		static constexpr std::string_view DisplayName = "Transform";

		static void Render(Fusion::Shared<Fusion::Actor> InActor, Fusion::TransformComponent* InComp);
	};

	template<>
	class ComponentUI<Fusion::CameraComponent>
	{
	public:
		static constexpr std::string_view DisplayName = "Camera";

		static void Render(Fusion::Shared<Fusion::Actor> InActor, Fusion::CameraComponent* InComp);
	};

	template<>
	class ComponentUI<Fusion::MeshComponent>
	{
	public:
		static constexpr std::string_view DisplayName = "Mesh";

		static void Render(Fusion::Shared<Fusion::Actor> InActor, Fusion::MeshComponent* InComp);
	};

	template<>
	class ComponentUI<Fusion::PhysicsBodyComponent>
	{
	public:
		static constexpr std::string_view DisplayName = "Physics Body";

		static void Render(Fusion::Shared<Fusion::Actor> InActor, Fusion::PhysicsBodyComponent* InComp);
	};

	template<>
	class ComponentUI<Fusion::SphereShapeComponent>
	{
	public:
		static constexpr std::string_view DisplayName = "Sphere Shape";

		static void Render(Fusion::Shared<Fusion::Actor> InActor, Fusion::SphereShapeComponent* InComp);
	};

	template <>
	class ComponentUI<Fusion::BoxShapeComponent>
	{
	public:
		static constexpr std::string_view DisplayName = "Box Shape";

		static void Render(Fusion::Shared<Fusion::Actor> InActor, Fusion::BoxShapeComponent* InComp);
	};

}
