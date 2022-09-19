#pragma once

#include "Fusion/World/Actor.h"
#include "Fusion/World/Components/CoreComponents.h"

namespace FusionEditor {

	template<typename TComponent>
	class ComponentRenderer
	{
	public:
		static void Render(Fusion::Shared<Fusion::Actor> InActor, TComponent* InComp);
	};

	template<>
	class ComponentRenderer<Fusion::TransformComponent>
	{
	public:
		static void Render(Fusion::Shared<Fusion::Actor> InActor, Fusion::TransformComponent* InComp);

	};

}
