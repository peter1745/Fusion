#pragma once

#include "Actor.h"
#include "Components/CoreComponents.h"

#include <vector>
#include <EnTT/entt.hpp>

namespace Fusion {

	class World
	{
	public:
		World();

		Shared<Actor> CreateActor(const std::string& InName, const Shared<Actor>& InParent = nullptr);
		Shared<Actor> FindActorWithID(ActorID InActorID) const;

		template<typename TComponent, typename... TComponentParams>
		TComponent* AddActorComponent(ActorID InActorID, TComponentParams&&... InComponentParams)
		{
			if (m_ActorIDMap.find(InActorID) == m_ActorIDMap.end())
			{
				FUSION_CORE_WARN("Tried to add component to an invalid actor!");
				return nullptr;
			}

			return &m_Registry.get_or_emplace<TComponent>(m_ActorIDMap.at(InActorID), std::forward<TComponentParams>(InComponentParams)...);
		}

		template<typename TComponent>
		TComponent* FindActorComponent(ActorID InActorID)
		{
			if (m_ActorIDMap.find(InActorID) == m_ActorIDMap.end())
			{
				FUSION_CORE_WARN("Tried to add component to an invalid actor!");
				return nullptr;
			}

			return m_Registry.try_get<TComponent>(m_ActorIDMap.at(InActorID));
		}

		template<typename TComponent>
		const TComponent* FindActorComponent(ActorID InActorID) const
		{
			if (m_ActorIDMap.find(InActorID) == m_ActorIDMap.end())
			{
				FUSION_CORE_WARN("Tried to add component to an invalid actor!");
				return nullptr;
			}

			return m_Registry.try_get<TComponent>(m_ActorIDMap.at(InActorID));
		}

		template<typename... TComponents>
		std::vector<Shared<Actor>> FindAllActorsWith() const
		{
			auto EnttView = m_Registry.view<ActorComponent, TComponents...>();
			std::vector<Shared<Actor>> Result;
			Result.reserve(EnttView.size_hint());

			for (auto EnttID : EnttView)
			{
				const auto& ActorComp = m_Registry.get<ActorComponent>(EnttID);
				Result.push_back(m_Actors.at(ActorComp.ID));
			}

			return Result;
		}

		Shared<Actor> GetMainCameraActor() const;

	private:
		entt::registry m_Registry;
		std::unordered_map<ActorID, entt::entity> m_ActorIDMap;
		std::unordered_map<ActorID, Shared<Actor>> m_Actors;
	};

}
