#pragma once

#include "BaseComponent.h"
#include "Fusion/World/Actor.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Fusion {

	struct ActorComponent : public BaseComponent
	{
		ActorID ID;
	};

	struct TransformComponent : public BaseComponent
	{
		glm::vec3 Location = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);
		glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	};

}
