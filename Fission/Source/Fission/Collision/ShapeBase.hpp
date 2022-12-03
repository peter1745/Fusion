#pragma once

#include "AABB.hpp"

namespace Fission {

	enum class EShapeType { None, Sphere, Box };

	class ShapeBase
	{
	public:
		virtual ~ShapeBase() = default;

		[[nodiscard]] EShapeType GetType() const { return m_Type; }
		
		[[nodiscard]] const glm::vec3& GetCenterOfMass() const { return m_CenterOfMass; }

		[[nodiscard]] float GetRestitution() const { return m_Restitution; }

		void SetRestitution(float InRestitution)
		{
			m_Restitution = glm::clamp(InRestitution, 0.0f, 1.0f);
		}

		[[nodiscard]]float GetFriction() const { return m_Friction; }

		void SetFriction(float InFriction)
		{
			m_Friction = glm::clamp(InFriction, 0.0f, 1.0f);
		}

		[[nodiscard]] virtual float GetFastestLinearSpeed(const glm::vec3& InAngularVelocity, const glm::vec3& InDirection) const = 0;
		[[nodiscard]] virtual glm::vec3 GetFurthestPoint(const glm::vec3& InLocation, const glm::vec3& InDirection, const glm::quat& InOrientation, float InBias) const = 0;

		virtual void CalculateInertiaTensor() = 0;

		[[nodiscard]] const AABB& GetBoundingBox() const { return m_BoundingBox; }
		[[nodiscard]] virtual AABB GetBoundingBox(const glm::vec3& InLocation, const glm::quat& InOrientation) const;

		[[nodiscard]] const glm::mat3& GetInertiaTensor() const { return m_InertiaTensor; }

	protected:
		ShapeBase() = default;
		explicit ShapeBase(EShapeType InType)
			: m_Type(InType) {}

	protected:
		glm::vec3 m_CenterOfMass = { 0.0f, 0.0f, 0.0f };
		float m_Restitution = 0.5f;
		float m_Friction = 0.1f;
		glm::mat3 m_InertiaTensor = glm::mat3(0.0f);

		AABB m_BoundingBox = {};

	private:
		EShapeType m_Type = EShapeType::None;
	};

}
