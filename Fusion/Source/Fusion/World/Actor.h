#pragma once

#include "Fusion/Core/Core.h"
#include "Fusion/Memory/Shared.h"
#include "Fusion/Core/UUID.h"

namespace Fusion {

	class World;

	using ActorID = UUID;

	class Actor : public SharedObject
	{
	public:
		const ActorID& GetActorID() const { return m_ActorID; }

		template<typename TComponent, typename... TComponentParams>
		TComponent* AddComponent(TComponentParams&&... InComponentParams)
		{
			return m_World->AddActorComponent<TComponent>(m_ActorID, std::forward<TComponentParams>(InComponentParams)...);
		}

		template<typename TComponent, typename... TComponentParams>
		TComponent* AddOrReplaceComponent(TComponentParams&&... InComponentParams)
		{
			return m_World->AddOrReplaceActorComponent<TComponent>(m_ActorID, std::forward<TComponentParams>(InComponentParams)...);
		}

		template<typename TComponent>
		TComponent* FindComponent() { return m_World->FindActorComponent<TComponent>(m_ActorID); }

		template<typename TComponent>
		const TComponent* FindComponent() const { return m_World->FindActorComponent<TComponent>(m_ActorID); }

	private:
		Actor(World* InWorld, ActorID InActorID = ActorID());

	public:
		std::string Name = "No Name";

	private:
		ActorID m_ActorID;
		World* m_World = nullptr;

		friend class World;
	};

}
