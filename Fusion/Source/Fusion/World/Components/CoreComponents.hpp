#pragma once

#include "BaseComponent.hpp"
#include "ComponentAttributes.hpp"
#include "Fusion/World/Actor.hpp"
#include "Fusion/Renderer/WorldCamera.hpp"
#include "Fusion/AssetSystem/Asset.hpp"
//#include "Fusion/Renderer/Texture.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Fusion {

	// NOTE(Peter): Is this component necessary
	struct ActorComponent
	{
		ActorID ID;
	};

	struct TransformComponent
	{
		glm::vec3 Location = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);

		void SetRotation(const glm::quat& InRotation)
		{
			m_Rotation = InRotation;
			m_RotationEuler = glm::eulerAngles(InRotation);
		}

		void SetRotationEuler(const glm::vec3& InRotation)
		{
			m_Rotation = glm::quat(InRotation);
			m_RotationEuler = InRotation;
		}

		const glm::quat& GetRotation() const { return m_Rotation; }
		const glm::vec3& GetRotationEuler() const { return m_RotationEuler; }

	private:
		glm::quat m_Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 m_RotationEuler = { 0.0f, 0.0f, 0.0f };
	};

	template<>
	struct ComponentAttributes<TransformComponent>
	{
		using ComponentType = TransformComponent;

		static constexpr bool CanDisable = false;
		static constexpr bool Removable = false;
	};

	struct RelationshipComponent
	{
		ActorID FirstChildID = ActorID::Invalid;
		ActorID NextSiblingID = ActorID::Invalid;
		ActorID PrevSiblingID = ActorID::Invalid;
		ActorID ParentID = ActorID::Invalid;
	};

	struct CameraComponent : public BaseComponent
	{
		WorldCamera CameraInstance;
		bool IsMainCamera = true;
	};

	struct MeshComponent : public BaseComponent
	{
		AssetHandle MeshHandle;
		//Shared<Texture2D> Texture = nullptr;
	};

	struct PhysicsBodyComponent : public BaseComponent
	{
		float Mass = 1.0f;
	};

	struct BoxShapeComponent : public BaseComponent
	{
		glm::vec3 HalfSize = { 0.5f, 0.5f, 0.5f };
	};

	struct SphereShapeComponent : public BaseComponent
	{
		float Radius = 0.5f;
	};

}
