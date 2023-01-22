#pragma once

#include "Integrator.hpp"

namespace Fission {

	class SemiImplicitEulerIntegrator : public Integrator
	{
	public:
		void IntegrateForceTorqueAndDrag(DynamicBody* InBody, const glm::vec3& InGravity, float InDeltaTime) override;
		void IntegrateVelocity(DynamicBody* InBody, float InDeltaTime) override;
	};

}
