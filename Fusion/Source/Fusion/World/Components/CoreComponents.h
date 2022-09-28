#pragma once

#include "BaseComponent.h"
#include "Fusion/World/Actor.h"

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

	enum class EProjectionType { PerspectiveProjection, OrthographicProjection };
	
	struct CameraComponent : public BaseComponent
	{
		EProjectionType ProjectionType = EProjectionType::PerspectiveProjection;

		// General Parameters
		float NearPlane = 0.1f;
		float FarPlane = 1000.0f;

		// Perspective Parameters
		float VerticalFOV = 70.0f;

		// Orthographic Parameters
		float OrthographicSize = 10.0f;
	};

}
