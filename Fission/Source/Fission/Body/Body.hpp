#pragma once

#include "BodyID.hpp"

#include "Fission/Collision/ShapeBase.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Fission {

	struct BodySettings
	{
		glm::vec3 InitialLocation;
		glm::quat InitialOrientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		float Mass;
		ShapeBase* Shape = nullptr;
	};

	struct Body
	{
		BodyID ID = BodyID::InvalidID;
		glm::vec3 Location = { 0.0f, 0.0f, 0.0f };
		glm::quat Orientation = { 1.0f, 0.0f, 0.0f, 0.0f };

		float InverseMass = 0.0f;
		ShapeBase* Shape = nullptr;

		glm::vec3 LinearVelocity = { 0.0f, 0.0f, 0.0f };
		glm::vec3 AngularVelocity = { 0.0f, 0.0f, 0.0f };
	};

}
