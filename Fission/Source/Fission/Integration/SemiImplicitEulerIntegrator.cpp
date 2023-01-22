#include "FissionPCH.hpp"
#include "SemiImplicitEulerIntegrator.hpp"

namespace Fission {

	void SemiImplicitEulerIntegrator::IntegrateForceTorqueAndDrag(DynamicBody* InBody, const glm::vec3& InGravity, float InDeltaTime)
	{
		glm::vec3 LinearVelocity = InBody->GetLinearVelocity();
		LinearVelocity += (InGravity + InBody->GetAccumulatedForce() * InBody->GetInverseMass()) * InDeltaTime;
		InBody->SetLinearVelocity(LinearVelocity);
	}

	void SemiImplicitEulerIntegrator::IntegrateVelocity(DynamicBody* InBody, float InDeltaTime)
	{
		glm::vec3 Position = InBody->GetPosition();
		InBody->SetPosition(Position + InBody->GetLinearVelocity() * InDeltaTime);
	}

}
