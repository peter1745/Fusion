#pragma once

#include "CoreComponents.h"
#include "Components2D.h"

namespace Fusion {

	template<typename... TComponents>
	struct ComponentGroup {};

	using AllComponents = ComponentGroup<TransformComponent, SpriteComponent, CameraComponent>;

	namespace ComponentUtils {

		template<typename TFunc, typename... TComponents>
		inline static void Each(Shared<Actor> InActor, TFunc InFunc)
		{
			([&]()
			{
				auto* Comp = InActor->FindComponent<TComponents>();
				if (Comp != nullptr)
					InFunc(Comp);
			}(), ...);
		}

		template<typename TFunc, typename... TComponents>
		inline static void Each(Shared<Actor> InActor, ComponentGroup<TComponents...> InCompGroup, TFunc InFunc)
		{
			Each<TFunc, TComponents...>(InActor, InFunc);
		}

		template<typename TFunc>
		inline static void All(Shared<Actor> InActor, TFunc InFunc)
		{
			Each<TFunc>(InActor, AllComponents{}, InFunc);
		}

	}

}
