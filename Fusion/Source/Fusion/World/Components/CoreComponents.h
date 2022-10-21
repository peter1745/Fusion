#pragma once

#include "BaseComponent.h"
#include "Fusion/World/Actor.h"
#include "Fusion/Renderer/WorldCamera.h"
#include "Fusion/Renderer/Mesh.h"
#include "Fusion/Renderer/Texture.h"

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
		Shared<Mesh> Mesh = nullptr;
		Shared<Texture2D> Texture = nullptr;
	};

}
