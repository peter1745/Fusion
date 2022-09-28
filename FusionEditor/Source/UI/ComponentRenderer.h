#pragma once

#include "Fusion/World/Actor.h"
#include "Fusion/World/Components/CoreComponents.h"
#include "Fusion/World/Components/Components2D.h"

namespace FusionEditor {

	template<typename TComponent>
	class ComponentUI
	{
	public:
		static void Render(Fusion::Shared<Fusion::Actor> InActor, TComponent* InComp) { static_assert(false); }
	};

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

	private:
		static void RenderPerspectiveParameters(Fusion::CameraComponent* InComp);
		static void RenderOrthographicParameters(Fusion::CameraComponent* InComp);
	};

}
