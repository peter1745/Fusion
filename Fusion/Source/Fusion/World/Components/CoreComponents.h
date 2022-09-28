#pragma once

#include "BaseComponent.h"
#include "Fusion/World/Actor.h"
#include "Fusion/Renderer/Camera.h"
#include "Fusion/Renderer/Mesh.h"

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
		glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
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
		Camera CameraInstance;
		bool IsMainCamera = true;

		EProjectionType ProjectionType = EProjectionType::PerspectiveProjection;

		// General Parameters
		float NearPlane = 0.1f;
		float FarPlane = 1000.0f;

		// Perspective Parameters
		float VerticalFOV = 70.0f;

		// Orthographic Parameters
		float OrthographicSize = 10.0f;
	};

	struct MeshComponent : public BaseComponent
	{
		Shared<Mesh> Mesh = nullptr;
	};

}
