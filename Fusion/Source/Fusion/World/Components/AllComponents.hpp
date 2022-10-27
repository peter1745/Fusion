#pragma once

#include "CoreComponents.hpp"
#include "Components2D.hpp"

namespace Fusion {

	template<typename... TComponents>
	struct ComponentGroup {};

	using AllComponents = ComponentGroup<TransformComponent, SpriteComponent, CameraComponent, MeshComponent>;

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
		inline static void EachConst(const Shared<Actor>& InActor, TFunc InFunc)
		{
			([&]()
			{
				const auto* Comp = InActor->FindComponent<TComponents>();
				if (Comp != nullptr)
					InFunc(Comp);
			}(), ...);
		}

		template<typename TFunc, typename... TComponents>
		inline static void Each(Shared<Actor> InActor, [[maybe_unused]] ComponentGroup<TComponents...> InCompGroup, TFunc InFunc)
		{
			Each<TFunc, TComponents...>(InActor, InFunc);
		}

		template<typename TFunc, typename... TComponents>
		inline static void EachConst(const Shared<Actor>& InActor, [[maybe_unused]] ComponentGroup<TComponents...> InCompGroup, TFunc InFunc)
		{
			EachConst<TFunc, TComponents...>(InActor, InFunc);
		}

		template<typename TFunc>
		inline static void All(Shared<Actor> InActor, TFunc InFunc)
		{
			Each<TFunc>(InActor, AllComponents{}, InFunc);
		}

		template<typename TFunc>
		inline static void AllConst(const Shared<Actor>& InActor, TFunc InFunc)
		{
			EachConst<TFunc>(InActor, AllComponents{}, InFunc);
		}

	}

}
