#pragma once

#include "Fission/Body/Body.hpp"

#include <vector>

namespace Fission {

	struct ContactPair
	{
		glm::vec3 WorldSpacePointA; // Hit point on body A in world space
		glm::vec3 WorldSpacePointB; // Hit point on body B in world space
		
		glm::vec3 Normal; // Contact normal in world space
		float SeparationDistance; // Positive when non-penetrating, Negative when penetrating

		float ImpactTime;

		Body* BodyA;
		Body* BodyB;
	};

	class ContactResolver
	{
	public:
		void GenerateContact(Body* InBodyA, Body* InBodyB);

		void ResolveContacts();

	private:
		glm::vec3 GetContactPosition(const glm::vec3& InBodyPosition, const glm::vec3& InNormal, const Shape* InShape) const;

	private:
		std::vector<ContactPair> m_Contacts;
	};

}
