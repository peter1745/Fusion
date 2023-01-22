#pragma once

#include "Fission/Collision/Shapes/Shape.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>

namespace Fission {

	enum class EBodyType : uint8_t { Static, Dynamic, Kinematic };

	struct BodySettings
	{
		EBodyType BodyType = EBodyType::Dynamic;
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::quat Rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		float Mass = 1000.0f;
		Shape* CollisionShape = nullptr;
	};

	class Body
	{
	public:
		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& InPosition) { m_Position = InPosition; }

		virtual constexpr EBodyType GetType() const { return EBodyType::Static; }

		template<typename TBodyType>
		TBodyType* As()
		{
			return static_cast<TBodyType*>(this);
		}

	public:
		Shape* GetShape() const { return m_Shape; }

	private:
		glm::vec3 m_Position;
		glm::quat m_Rotation;

		// TODO(Peter): Make this a unique_ptr, body should own it's shape
		Shape* m_Shape = nullptr;

	private:
		friend class PhysicsWorld;
	};

}
