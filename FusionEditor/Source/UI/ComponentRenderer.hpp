#pragma once

#include "Fusion/World/Actor.hpp"
#include "Fusion/World/Components/CoreComponents.hpp"
#include "Fusion/World/Components/Components2D.hpp"

namespace FusionEditor {

	template<typename TComponent>
	class ComponentUI {};

	template<>
	class ComponentUI<Fusion::TransformComponent>
	{
	public:
		static void Render(Fusion::Shared<Fusion::Actor> InActor, Fusion::TransformComponent* InComp);
	};

	template<>
	class ComponentUI<Fusion::SpriteComponent>
	{
	public:
		static void Render(Fusion::Shared<Fusion::Actor> InActor, Fusion::SpriteComponent* InComp);
	};

	template<>
	class ComponentUI<Fusion::CameraComponent>
	{
	public:
		static void Render(Fusion::Shared<Fusion::Actor> InActor, Fusion::CameraComponent* InComp);
	};

	template<>
	class ComponentUI<Fusion::MeshComponent>
	{
	public:
		static void Render(Fusion::Shared<Fusion::Actor> InActor, Fusion::MeshComponent* InComp);
	};

}
