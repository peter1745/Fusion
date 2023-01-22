#pragma once

#include "Fission/Body/DynamicBody.hpp"

namespace Fission {

	class Integrator
	{
	public:
		virtual void IntegrateForceTorqueAndDrag(DynamicBody* InBody, const glm::vec3& InGravity, float InDeltaTime) = 0;
		virtual void IntegrateVelocity(DynamicBody* InBody, float InDeltaTime) = 0;
	};

}
