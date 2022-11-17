#pragma once

#include "Fusion/Core/Core.hpp"
#include "Fusion/Memory/Shared.hpp"
#include "Fusion/Core/UUID.hpp"

namespace Fusion {

	class World;

	using ActorID = UUID;

	class Actor : public SharedObject
	{
	public:
		const ActorID& GetActorID() const { return m_ActorID; }

		template<typename TComponent, typename... TComponentParams>
		TComponent* AddComponent(TComponentParams&&... InComponentParams);

		template<typename TComponent, typename... TComponentParams>
		TComponent* AddOrReplaceComponent(TComponentParams&&... InComponentParams);

		template<typename TComponent>
		TComponent* FindComponent();

		template<typename TComponent>
		const TComponent* FindComponent() const;

		template<typename TComponent>
		void RemoveComponent();

	private:
		Actor(World* InWorld, ActorID InActorID = ActorID());

	public:
		std::string Name = "No Name";

	private:
		ActorID m_ActorID;
		World* m_World = nullptr;

		friend class World;
	};

	using SharedActor = Shared<Actor>;

}
