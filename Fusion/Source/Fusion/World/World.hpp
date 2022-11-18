#pragma once

#include "Actor.hpp"
#include "Components/CoreComponents.hpp"

#include "Fusion/Core/Enum.hpp"

#include <Fission/World/DynamicsWorld.hpp>

#include <vector>
#include <EnTT/entt.hpp>

namespace Fusion {

	using EWorldState = Flags<>;

	namespace WorldStates
	{
		static constexpr EWorldState None = (1 << 0);
		static constexpr EWorldState Simulating = (1 << 1);
	}

	class World : public SharedObject
	{
	public:
		World(const std::string& InName = "Empty World");
		~World();

		void StartSimulation();
		void Simulate(float InDeltaTime);
		void StopSimulation();

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& InName) { m_Name = InName; }

		Shared<Actor> CreateActorWithID(ActorID InActorID, const std::string& InName, const Shared<Actor>& InParent = nullptr);
		Shared<Actor> CreateActor(const std::string& InName, const Shared<Actor>& InParent = nullptr)
		{
			return CreateActorWithID(ActorID(), InName, InParent);
		}

		Shared<Actor> FindActorWithID(ActorID InActorID) const;

		void DestroyActor(ActorID InActorID);

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

		template<typename TComponent, typename... TComponentParams>
		TComponent* AddOrReplaceActorComponent(ActorID InActorID, TComponentParams&&... InComponentParams)
		{
			if (m_ActorIDMap.find(InActorID) == m_ActorIDMap.end())
			{
				FUSION_CORE_WARN("Tried to add component to an invalid actor!");
				return nullptr;
			}

			return &m_Registry.emplace_or_replace<TComponent>(m_ActorIDMap.at(InActorID), std::forward<TComponentParams>(InComponentParams)...);
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

		template<typename TComponent>
		void RemoveActorComponent(ActorID InActorID)
		{
			if (m_ActorIDMap.find(InActorID) == m_ActorIDMap.end())
			{
				FUSION_CORE_WARN("Tried to remove component from an invalid actor!");
				return;
			}

			m_Registry.remove<TComponent>(m_ActorIDMap.at(InActorID));
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

		void Restore(Shared<World> InOther);

		uint32_t GetEnttID(ActorID InActorID) const { return static_cast<uint32_t>(m_ActorIDMap.at(InActorID)); }

		Shared<Actor> GetMainCameraActor() const;

		void SetState(EWorldState InState) { m_State = InState; }
		EWorldState GetState() const { return m_State; }

		void Clear();

	public:
		static Shared<World> Copy(Shared<World> InOther);

	private:
		std::string m_Name = "Empty World";

		EWorldState m_State = WorldStates::None;

		entt::registry m_Registry;
		std::unordered_map<ActorID, entt::entity> m_ActorIDMap;
		std::unordered_map<ActorID, Shared<Actor>> m_Actors;

		Fission::DynamicsWorld m_PhysicsWorld;
		std::unordered_map<ActorID, Fission::BodyID> m_ActorIDToPhysicsBodyIDMap;
	};

	template<typename TComponent, typename... TComponentParams>
	TComponent* Actor::AddComponent(TComponentParams&&... InComponentParams)
	{
		return m_World->AddActorComponent<TComponent>(m_ActorID, std::forward<TComponentParams>(InComponentParams)...);
	}

	template<typename TComponent, typename... TComponentParams>
	TComponent* Actor::AddOrReplaceComponent(TComponentParams&&... InComponentParams)
	{
		return m_World->AddOrReplaceActorComponent<TComponent>(m_ActorID, std::forward<TComponentParams>(InComponentParams)...);
	}

	template<typename TComponent>
	TComponent* Actor::FindComponent() { return m_World->FindActorComponent<TComponent>(m_ActorID); }

	template<typename TComponent>
	const TComponent* Actor::FindComponent() const { return m_World->FindActorComponent<TComponent>(m_ActorID); }

	template<typename TComponent>
	void Actor::RemoveComponent() { m_World->RemoveActorComponent<TComponent>(m_ActorID); }

}
