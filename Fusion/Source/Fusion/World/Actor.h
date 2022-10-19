#pragma once

#include "Fusion/Core/Core.h"
#include "Fusion/Memory/Shared.h"

namespace Fusion {

	class ActorID
	{
	public:
		static const uint64_t Invalid = 0;

	public:
		ActorID();
		ActorID(uint64_t InActorID);
		ActorID(const ActorID& InOther);

		operator uint64_t() { return m_ActorID; }
		operator const uint64_t() const { return m_ActorID; }
	private:
		uint64_t m_ActorID;
	};

	class World;

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

namespace std {

	template<>
	struct hash<Fusion::ActorID>
	{
		size_t operator()(const Fusion::ActorID& InActorID) const noexcept
		{
			return (uint64_t)InActorID;
		}
	};

}
