#pragma once

#include "Body.hpp"

namespace Fission {

	class DynamicBody : public Body
	{
	public:
		constexpr EBodyType GetType() const override { return EBodyType::Dynamic; }

		const glm::vec3& GetLinearVelocity() const { return m_LinearVelocity; }
		void SetLinearVelocity(const glm::vec3& InLinearVelocity) { m_LinearVelocity = InLinearVelocity; }

		float GetInverseMass() const { return m_InvMass; }
		float GetMass() const { return 1.0f / m_InvMass; }
		void SetMass(float InMass) { m_InvMass = 1.0f / InMass; }

		const glm::vec3& GetAccumulatedForce() const { return m_Force; }

	private:
		float m_InvMass;
		glm::vec3 m_LinearVelocity = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Force = { 0.0f, 0.0f, 0.0f };

	private:
		friend class PhysicsWorld;
	};

}
